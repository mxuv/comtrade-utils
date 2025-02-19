#include <stdio.h>

#include "strings.h"
#include "format.h"

#define STR_BUFSIZE             4096

enum getstring_status
{
    gss_ok,
    gss_empty,
    gss_eof,
    gss_err,
    gss_overflow
};

/*  read string from file
    return: string length, status
*/
int getstring(FILE *fd, char *buffer, int bufsize,
                enum getstring_status *status)
{
    int len = 0;

    *status = gss_ok;
    if (fgets(buffer, bufsize, fd)) {
        len = stringlen(buffer);
        if (buffer[len-1] != '\n') {
            if (len == bufsize - 1)
                *status  = gss_overflow;
        }
    } 
    if (feof(fd))
        *status = gss_eof;
    if (ferror(fd))
        *status = gss_err;

    return len;
}

int check_line_ending(const char *str, int len)
{
    if (str[len-2] == '\r')
        return 1;
    else
        return 0;
}

int get_line_param_count(const char *str, int len)
{
    int count = 0;
    while (len) {
        if (*str == ',')
            count++;
        str++;
        len--;
    }
    return count;
}

int match_char(char ch, char patt)
{
    if (ch == patt)
        return 1;
    else
        return 0;
}

void extract_parameter_from_string(char *buffer, const char *str)
{
    while ((*str != ',') && (*str != '\r')) {
        *buffer = *str;
        str++;
        buffer++;
    }
    *buffer = 0;
}

int get_param_index(const char *str, int param)
{
    int param_curr = 1;
    const char *p;

    p = str;
    while (param_curr != param) {
        if (match_char(*str, ','))
            param_curr++;
        str++;
    }
    return str - p;
}

void read_header()
{
}

enum analyze_cfg_state
{
    analyze_header,
    analyze_tt,
    analyze_ach,
    analyze_dch,
    analyze_lf,
    analyze_nrates,
    analyze_samp,
    analyze_sdatetime,
    analyze_trigdatetime,
    analyze_filetype,
    analyze_timemult,
    analyze_timecode,
    analyze_tmqcode
}

int analyze_cfgfile(FILE *fd, cmtrd_cfg_body_t *cfg_body)
{
    char buffer[STR_BUFSIZE];
    int strlen, current_line, lines_in_file = 2;
    enum getstring_status status;
    enum analyze_cfg_state next_state = analyze_header;


    while (strlen = getstring(fd, buffer, STR_BUFSIZE, &status)) {
    }
    /* while (current_line < lines_in_file){ */
    /*     strlen = getstring(fd, buffer, STR_BUFSIZE, &status); */
    /*     if (status == gss_eof && (current_line + 1) != lines_in_file) */
    /* } */

    return 0;
}

/* return codes:
 * 0 - Ok
 * 1 - file read error
*/
int main(int argc, char **argv)
{
    FILE *fd;
    /* enum getstring_status status; */
    /* char buffer[4096]; */
    /* char str[64]; */
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

    analyze_cfgfile(fd, &cfg_body);
    /* int len, count; */
    /* len = getstring(fd, buffer, 4096, &status); */
    /* len = getstring(fd, buffer, 4096, &status); */
    /* len = getstring(fd, buffer, 4096, &status); */
    /* if (check_line_ending(buffer, len)) */
    /*     fputs("<CR><LF> OK\n", stdout); */
    /* else */
    /*     fputs("NO <CR>\n", stdout); */
    /*  */
    /* count = get_line_param_count(buffer, len); */
    /* count++; */
    /* printf("Line have a %d parametrs\n", count); */
    /* for (int i = 0; i < count; i++) {  */
    /*     int index = get_param_index(buffer, i+1); */
    /*     printf("Index of %d parameter is %d\n", i+1, index); */
    /*     extract_parameter_from_string(str, buffer + index); */
    /*     printf("Parameter [%d]: %s\n", i+1, str); */
    /* } */

    fclose(fd);
    return 0;
} 


/*
    int len = getstring(fd, buffer, 4094, &status);
    switch (status) {
    case gss_ok:
        printf(buffer);
        printf("%d\n", len);
        break;
    case gss_empty:
        fputs("Empty string\n", stderr);
        break;
    case gss_eof:
        fputs("End of file\n", stderr);
        break;
    case gss_err:
        perror(argv[1]);
        break;
    case gss_overflow:
        fputs("Buffer overflow\n", stderr);
        break;
    default:
        break;
    }

*/
