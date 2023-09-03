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
    fclose(file);
}

