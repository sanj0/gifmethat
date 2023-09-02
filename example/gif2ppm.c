#include <stdio.h>

#include "../src/gifmethat.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "expected as arguments a source and a destination file!\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        fprintf(stderr, "file %s does not exist!\n", argv[1]);
        return 1;
    }
    int fd = fileno(file);
    Gif gif;
    read_gif(fd, &gif);
    if (write_ppm(argv[2], &gif)) {
        fprintf(stderr, "cannot write to file %s!\n", argv[2]);
        return 1;
    }
    return 0;
}
