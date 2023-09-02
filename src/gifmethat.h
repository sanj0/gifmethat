#ifndef _GIFMETHAT_H
#define _GIFMETHAT_H

#define BLOCK_IMAGE_DESCRIPTOR  0x2C
#define BLOCK_EXTENSION         0x21
#define EXT_GRAPHIC_CONTROL     0xF9
#define BLOCK_TRAILER           0x3B
// Various error codes returned by the read functions
#define ERR_INVALID_GIF_ID      1
#define ERR_INVALID_GIF_VERSION 2
#define ERR_UNSUPPORTED_FEATURE 3

#include <stdlib.h>

// The valid GIF magic number, defined in gifmethat.c
extern const uint8_t VALID_ID[3];
// The valid version this library parses, defined in gifmethat.c
extern const uint8_t VALID_VERSION[3];

// A Color, defined by one byte red, green and blue each.
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

// The header of a GIF file according to the specification. This library only
// uses width, height and some of the `fields` bits. `id` and `version` are
// only parsed to check against the above declared constants. The rest is only
// parsed for completion's sake and to properly advance the file cursor.
typedef struct {
    uint8_t id[3];
    uint8_t version[3];
    uint16_t width;
    uint16_t height;
    uint8_t fields;
    uint8_t bg_index;
    uint8_t pixel_aspect_ratio;
} Header;

// A decoded Gif. Consists of an array of `Color`s and a widht and height.
// Ideally, and always if the Gif was read by this library, the length of the
// `pixels` array will be `width * height`.
// The `pixels` array lists the pixels in the image from top left to bottom
// right, row for row, meaning the first `width` pixels make up the first row
// of the image from left to right and so on.
typedef struct {
    Color *pixels;
    uint16_t width;
    uint16_t height;
} Gif;

// Reads a `Color` from the given file descriptor into the memory location
// provided.
// Returns 0 upon success and something else when something goes wrong.
// Currently doesn't care nor fail if the file doesn't have enough data left.
int read_color(int fd, Color *color);

// Reads a `Header` from the given file descriptor into the memory location
// provided.
// Returns 0 upon success and on of the error numbers defined above when
// something goes wrong. Currently doesn't care nor fail if the file doesn't
// have enough data left.
int read_header(int fd, Header *header);

// Reads a `Gif` from the given file descriptor into the memory location
// provided.
// Returns 0 upon success and on of the error numbers defined above when
// something goes wrong. Currently doesn't care nor fail if the file doesn't
// have enough data left.
int read_gif(int fd, Gif *gif);

// Writes the given `Gif` as a ppm image into a file at the given location.
// Mainly here to test and debug the library but aside from that a neat little
// feature.
int write_ppm(const char *path, Gif *gif);

#endif
