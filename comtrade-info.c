#include <stdio.h>

#include "format.h"


int main(int argc, char **argv)
{
    FILE *fd;
    cmtrd_cfg_body_t cfg_body;

    if (argc < 2) {
        fputs("No input file\n", stderr);
        fputs("Nothing to do\n", stderr);
        return 1;
    }

    fd = fopen(argv[1], "r");
    if (fd == NULL) {
        perror(argv[1]);
        return 1;
    }

    fclose(fd);
    return 0;
} 
