#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "../src/gifmethat.h"

void print_header(Header* h);

int main(void) {
    FILE *file = fopen("cats.gif", "r");
    if (file == NULL) {
        return 1;
    }
    int fd = fileno(file);

    Gif cats;
    if (read_gif(fd, &cats))
        return -1;
    printf("%" PRIu16 "x%" PRIu16 "\n", cats.width, cats.height);
    write_ppm("cats.ppm", &cats);
}


void print_header(Header* h) {
    printf("Header = {\n\tid: %c%c%c\n\tversion: %c%c%c\n\twidth: %" PRIu16 "\n\theight: %" PRIu16 "\n\tfield: %b\n\tbg_index: %" PRIu8 "\n\tpixel_aspect_ratio: %" PRIu8 "\n}\n",
            (char) h->id[0],
            (char) h->id[1],
            (char) h->id[2],
            (char) h->version[0],
            (char) h->version[1],
            (char) h->version[2],
            h->width,
            h->height,
            h->fields,
            h->bg_index,
            h->pixel_aspect_ratio);
}
