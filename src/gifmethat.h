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
#define ERR_MISSING_DATA        4

#include <stdlib.h>
#include <inttypes.h>

// The valid GIF magic number and version, defined in gifmethat.c
extern const uint8_t VALID_ID_AND_VERSION[6];

// A Color, defined by one byte red, green and blue each.
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Color;

// The header of a GIF file according to the specification.
// This only contains fields the library uses to decode gifs.
// All other fields a are omitted and simply skipped in `read_header`
// to have this struct not be padded.
typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t fields;
    uint8_t bg_index;
} Header;

// A decoded Gif. Consists of an array of `Color`s and a width and height.
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
