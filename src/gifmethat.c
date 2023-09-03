#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>

#include "gifmethat.h"

// All the functions and constants are documented in the gifmethat.h file.

const uint8_t VALID_ID_AND_VERSION[6] = "GIF89a";

// an entry in the LZW decompress dictionary
typedef struct {
    uint8_t byte;
    int prev;
    size_t len;
} DictEntry;

int parse_image_descriptor(int fd, Gif *gif, Color *colors, size_t num_colors);
size_t read_sub_blocks(int fd, uint8_t **data);
int uncompress(int code_len, uint8_t *input, int input_len, uint8_t *out);

int read_header(int fd, Header *header) {
    uint8_t id_and_version[6];
    if (read(fd, id_and_version, 6) != 6)
        return ERR_MISSING_DATA;
    if (memcmp(VALID_ID_AND_VERSION, id_and_version, 3) != 0)
        return ERR_INVALID_GIF_ID;
    if (memcmp(VALID_ID_AND_VERSION + 3, id_and_version + 3, 3) != 0)
        return ERR_INVALID_GIF_VERSION;

    if (read(fd, header, sizeof(Header)) != sizeof(Header)) {
        return ERR_MISSING_DATA;
    } else {
        // skip the pixel aspect ratio
        lseek(fd, 1, SEEK_CUR);
        return 0;
    }
}

int read_gif(int fd, Gif *gif) {
    Header header;
    int err = read_header(fd, &header);
    if (err) {
        return err;
    }

    gif->width = header.width;
    gif->height = header.height;

    if (header.fields & (1 << 7) != 1 << 7) {
        fprintf(stderr, "only gifs with gobal color tables are supported!");
        return ERR_UNSUPPORTED_FEATURE;
    }

    size_t num_colors = pow(2, (header.fields & 0b111) + 1);
    // the global color table
    Color *colors = malloc(num_colors * sizeof(Color));
    read(fd, colors, num_colors * sizeof(Color));

    Gif color_table;
    color_table.pixels = colors;
    color_table.width = num_colors;
    color_table.height = 1;
    write_ppm("table.ppm", &color_table);

    uint8_t block_type = 0;
    // read blocks until the trailer block marks the end of the gif
    while (block_type != BLOCK_TRAILER) {
        read(fd, &block_type, 1);
        switch (block_type) {
            case BLOCK_IMAGE_DESCRIPTOR:
                parse_image_descriptor(fd, gif, colors, num_colors);
                break;
            case BLOCK_TRAILER:
                break;
            case BLOCK_EXTENSION:
                uint8_t ext_type;
                read(fd, &ext_type, 1);
                switch (ext_type) {
                    case EXT_GRAPHIC_CONTROL:
                        // just skip it
                        uint8_t *buf = malloc(6);
                        read(fd, &buf, 6);
                        break;
                    default:
                        fprintf(stderr, "unknwon extension %" PRIu8 "!\n", ext_type);
                        return ERR_UNSUPPORTED_FEATURE;
                }
                #if 0
                uint8_t *extension_data;
                size_t extension_data_len = read_sub_blocks(fd, &extension_data);
                if (extension_data)
                    free(extension_data);
                #endif
                break;
            default:
                fprintf(stderr, "unknown block type %" PRIu8 "\n", block_type);
                return 1;
        }
    }

    return 0;
}

int parse_image_descriptor(int fd, Gif *gif, Color *colors, size_t num_colors) {
    // x, y, width, height of the block within the image.
    // Could be optimized by storing the values in a struct and reading the whole thing at once.
    uint16_t x, y;
    uint16_t w, h;
    uint8_t fields;
    read(fd, &x, 2);
    read(fd, &y, 2);
    read(fd, &w, 2);
    read(fd, &h, 2);
    read(fd, &fields, 1);
    if (fields & (1 << 6) == (1 << 6)) {
        fprintf(stderr, "cannot parse interlaced gifs");
        return ERR_UNSUPPORTED_FEATURE;
    }
    uint8_t lzw_code_size;
    uint8_t *compressed_data = NULL;
    uint8_t *uncompressed_data = NULL;
    read(fd, &lzw_code_size, 1);
    size_t compressed_data_len = read_sub_blocks(fd, &compressed_data);
    size_t uncompressed_data_length = w * h;
    uncompressed_data = malloc(uncompressed_data_length);
    uncompress(lzw_code_size, compressed_data, compressed_data_len, uncompressed_data);

    gif->pixels = (Color *) malloc(sizeof(Color) * w * h);
    for (int i = 0; i < w * h; i++) {
        Color color = colors[uncompressed_data[i]];
        gif->pixels[i] = color;
    }

    if (compressed_data) {
        free(compressed_data);
    }
    if (uncompressed_data) {
        free(uncompressed_data);
    }
}

size_t read_sub_blocks(int fd, uint8_t **data) {
    size_t data_len = 0;
    size_t index = 0;
    uint8_t block_size;
    *data = NULL;
    // data blocks until a zero-sized block occurs.
    while (1) {
        read(fd, &block_size, 1);
        if (block_size == 0) {
            break;
        }
        data_len += block_size;
        *data = realloc(*data, data_len);
        read(fd, *data + index, block_size);
        index += block_size;
    }
    return data_len;
}

// this function is largely copied from the article mentioned in the readme
int uncompress(int code_len, uint8_t *input, int input_len, uint8_t *out) {
    int i, bit;
    int code, prev = -1;
    DictEntry *dict;
    int dict_index;
    size_t mask = 0x01;
    int reset_code_len = code_len;
    int clear_code = 1 << code_len;
    int stop_code = clear_code + 1;
    dict = malloc(sizeof(DictEntry) * (1 << (code_len + 1)));
    // initializes the dictionary
    for (dict_index = 0; dict_index < (1 << code_len); dict_index++) {
        dict[dict_index].byte = dict_index;
        dict[dict_index].prev = -1;
        dict[dict_index].len = 1;
    }
    dict_index += 2;
    while (input_len) {
        code = 0;
        for (i = 0; i < (code_len + 1); i++) {
            bit = (*input & mask) ? 1 : 0;
            mask <<= 1;
            if (mask == 0x100) {
                mask = 1;
                input++;
                input_len--;
            }
            code = code | (bit << i);
        }

        if (code == clear_code) {
            code_len = reset_code_len;
            dict = realloc(dict, sizeof(DictEntry) * (1 << (code_len + 1)));
            for (dict_index = 0; dict_index < (1 << code_len); dict_index++) {
                dict[dict_index].byte = dict_index;
                dict[dict_index].prev = -1;
                dict[dict_index].len = 1;
            }
            dict_index += 2;
            prev = -1;
            continue;
        } else if (code == stop_code) {
            break;
        }

        if ((prev > -1) && (code_len < 12)) {
            if (code > dict_index) {
                return 1;
            }
            int ptr = code == dict_index ? prev : code;
            while (dict[ptr].prev != -1)
                ptr = dict[ptr].prev;
            dict[dict_index].byte = dict[ptr].byte;
            dict[dict_index].prev = prev;
            dict[dict_index].len = dict[prev].len + 1;
            dict_index += 1;

            if ((dict_index == (1 << (code_len + 1))) && (code_len < 11)) {
                code_len += 1;
                dict = realloc(dict, sizeof(DictEntry) * (1 << (code_len + 1)));
            }
        }
        prev = code;
        int match_len = dict[code].len;
        while (code != -1) {
            out[dict[code].len - 1] = dict[code].byte;
            if (dict[code].prev == code)
                return 1;
            code = dict[code].prev;
        }
        out += match_len;
    }
}

// writes the given gif as a ppm at the given file path
int write_ppm(const char *path, Gif *gif) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        return 1;
    }
    fprintf(file, "P6\n%d\n%d\n255\n", gif->width, gif-> height);
    int num_pixels = gif->width * gif->height;
    for (int i = 0; i < num_pixels; i++) {
        Color color = gif->pixels[i];
        uint8_t color_array[3] = { color.red, color.green, color.blue };
        fwrite(color_array, 1, 3, file);
    }
    fclose(file);
    return 0;
}

