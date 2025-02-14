#include <stdio.h>

#include "strings.h"
#include "format.h"

#define STR_BUFSIZE             4096

enum getstring_status
{
    gls_ok,
    gls_empty,
    gls_eof,
    gls_err,
    gls_overflow
};

int getstring(FILE *fd, char *buffer, int size, enum getstring_status *status)
{
    int len = 0;

    *status = gls_ok;
    fgets(buffer, size, fd);
    len = stringlen(buffer);
    if (len == 0)
        *status = gls_empty;
    if (buffer[len-1] != '\n') {
        if (feof(fd))
            *status = gls_eof;
        if (ferror(fd))
            *status = gls_err;
        if (len == size - 1)
            *status = gls_overflow;
    }
    return len;
}

void read_header()
{
}

void analyze_cfgfile(FILE *fd)
{
    char buffer[STR_BUFSIZE];
}

int main(int argc, char **argv)
{
    FILE *fd;
    enum getstring_status status;
    char buffer[4096];
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


/*
    int len = getstring(fd, buffer, 4094, &status);
    switch (status) {
    case gls_ok:
        printf(buffer);
        printf("%d\n", len);
        break;
    case gls_empty:
        fputs("Empty string\n", stderr);
        break;
    case gls_eof:
        fputs("End of file\n", stderr);
        break;
    case gls_err:
        perror(argv[1]);
        break;
    case gls_overflow:
        fputs("Buffer overflow\n", stderr);
        break;
    default:
        break;
    }

*/
