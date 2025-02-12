#include <stdio.h>

int main(int argc, char **argv)
{
    FILE *fd;

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
