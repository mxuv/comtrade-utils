#include <stdio.h>
#include <stdlib.h>

#include "strings.h"
#include "format.h"

#define STR_BUFSIZE             4096

#define EXIT_MEMERR()           exit(5)

enum getstring_status
{
    gss_ok,
    gss_empty,
    gss_eof,
    gss_err,
    gss_overflow
};

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
};

typedef struct
{
    const char *str;
    int strlen;
    int nstr;
    int param_count;
} cfgfile_string_t;

int match_char(char ch, char patt)
{
    if (ch == patt)
        return 1;
    else
        return 0;
}

/* int find_char_in_string(const char *str, int strlen, char c) */
/* { */
/*  */
/* } */

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

int is_line_ending_ok(const char *str, int len)
{
    if (str[len-2] == '\r')
        return 1;
    else
        return 0;
}

int get_param_count(const char *str, int len)
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

#if 0
void extract_parameter_from_string(char *buffer, const char *str)
{
    while ((*str != ',') && (*str != '\r')) {
        *buffer = *str;
        str++;
        buffer++;
    }
    *buffer = 0;
}
#endif

void get_param(char *dest, const char *str, int index, int len)
{
    stringcopy_c(dest, str+index, len);
}

int get_param_index(const char *str, int param)
{
    int param_curr = 0;
    const char *p;

    p = str;
    while (param_curr != param) {
        if (match_char(*str, ','))
            param_curr++;
        str++;
    }
    return str - p;
}

int get_param_length(const char *str, int strlen, int param, int param_count)
{
    int index;

    index = get_param_index(str, param);
    if ((param + 1) == param_count) {
        if (match_char(*(str+(strlen - 2)), '\r'))
            return strlen - 2 - index;
        else
            return strlen - 1 - index;
    }
    else
        return get_param_index(str, param + 1) - index - 1;
}

void get_all_param_len(const char *str, int strlen, int param_count, int *len)
{
    int i;

    for (i = 0; i < param_count; i++)
        len[i] = get_param_length(str, strlen, i, param_count);
}

void get_all_param_index(const char *str, int param_count, int *index)
{
    int i;

    for (i = 0; i < param_count; i++)
        index[i] = get_param_index(str, i);
}

void add_error_field(cmtrd_cfg_body_t *cfg_rec)
{
    int i;
    cmtrd_err_t *p;

    p = malloc((cfg_rec->errcount + 1) * sizeof(cmtrd_err_t));
    if (p == NULL)
        EXIT_MEMERR();

    (p+cfg_rec->errcount)->ln = 0;
    (p+cfg_rec->errcount)->strerr = 0;
    (p+cfg_rec->errcount)->paramerr = 0;
    for (i = 0; i < cfg_rec->errcount; i++)
        *(p+i) = *(cfg_rec->errors+i);
    free(cfg_rec->errors);
    cfg_rec->errors = p;
    cfg_rec->errcount++;
}

void add_error_code(int line, int strcode, int paramcode,
                    cmtrd_cfg_body_t *cfg_rec)
{
    int i;
    cmtrd_err_t *p = cfg_rec->errors;

    for (i = 0; i < cfg_rec->errcount; i++) {
        if ((p+i)->ln == line) { 
            (p+i)->strerr |= strcode;
            (p+i)->paramerr |= paramcode;
            return;
        }
    }

    add_error_field(cfg_rec);
    (cfg_rec->errors+cfg_rec->errcount-1)->ln = line;
    (cfg_rec->errors+cfg_rec->errcount-1)->strerr |= strcode;
    (cfg_rec->errors+cfg_rec->errcount-1)->paramerr |= paramcode;
}

int is_correct_param_length(int len, int min, int max)
{
    if (len >= min && len <= max)
        return 1;
    else
        return 0;
}

char* add_str_item(const char *src, int length)
{
    char *str;

    str  = malloc(sizeof(char) * length + 1);
    if (str == NULL)
        EXIT_MEMERR();

    stringcopy_c(str, src, length);
    return str;
}

int analyze_cfg_header(cfgfile_string_t *cfg_str, cmtrd_cfg_body_t *cfg_rec)
{
    int index[CP_HEADER];
    int len[CP_HEADER];

    get_all_param_index(cfg_str->str, cfg_str->param_count, index);
    get_all_param_len(cfg_str->str, cfg_str->strlen, cfg_str->param_count, len);

    if (cfg_str->param_count < CP_HEADER_MIN) {
        add_error_code(cfg_str->nstr, LN_ERR_TOO_FEW_PARAM, 0, cfg_rec); 
        return 1;
    }
    if (!is_correct_param_length(len[0], SNAME_LEN_MIN, SNAME_LEN_MAX))
        add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM_LEN,
                PM_ERR_SNAME, cfg_rec);
    if (len[0])
        cfg_rec->station_name = add_str_item(cfg_str->str+index[0], len[0]);
    if (len[1])
        cfg_rec->rec_dev_id = add_str_item(cfg_str->str+index[1], len[1]);

    if (cfg_str->param_count < CP_HEADER) {
        cfg_rec->rev_year = 1991;
    } else {

    }

}

#if 0
int analyze_cfg_header(const char *str, int strlen, int param_count, 
                        cmtrd_cfg_body_t *cfg_rec)
{ 
    int i;
    int index[CP_HEADER];
    int param_len[CP_HEADER];
    char s[REVYEAR_LEN_MAX + 1];
    for (i = 0; i < param_count; i++) {

    }
    index = get
}
#endif

/* Return values:
 * 0-Ok
 * 2-Unexcepted end of file
 * 3-Read error
 * 4-Buffer overflow
*/
int analyze_cfgfile(FILE *fd, cmtrd_cfg_body_t *cfg_rec)
{
    char buffer[STR_BUFSIZE];
    cfgfile_string_t cfg_str;
    enum getstring_status status;
    enum analyze_cfg_state next_state = analyze_header;

    cfg_str.nstr = 0;
    while ((cfg_str.strlen = getstring(fd, buffer, STR_BUFSIZE, &status))) {
        if (status)
            return status;
        if (!is_line_ending_ok(buffer, cfg_str.strlen))
            add_error_code(cfg_str.nstr, LN_ERR_NOCR, 0, cfg_rec);

        cfg_str.str = buffer;
        cfg_str.param_count = get_param_count(buffer, cfg_str.strlen);
        switch (next_state) {
        case analyze_header:
            analyze_cfg_header(&cfg_str, cfg_rec);
            next_state++;
            break;
        default:
            break;
        }
    }











    /* while (current_line < lines_in_file){ */
    /*     strlen = getstring(fd, buffer, STR_BUFSIZE, &status); */
    /*     if (status == gss_eof && (current_line + 1) != lines_in_file) */
    /* } */

    return 0;
}

void cfg_record_init(cmtrd_cfg_body_t *cfg_rec)
{
    char *p;
    int i;

    p = (char*)cfg_rec;
    for (i = 0; i < sizeof(cmtrd_cfg_body_t); i++)
        *(p + i) = 0;
    /* memset(cfg_rec, 0, sizeof(cmtrd_cfg_body_t)); */
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
    cmtrd_cfg_body_t cfg_rec;

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

    cfg_record_init(&cfg_rec);
    analyze_cfgfile(fd, &cfg_rec);
    printf("errcount = %d\n", cfg_rec.errcount);
    add_error_code(1, 2, 6, &cfg_rec);
    printf("errcount = %d\n", cfg_rec.errcount);
    add_error_code(2, 1, 16, &cfg_rec);
    printf("errcount = %d\n", cfg_rec.errcount);
    printf("%d, %d, %ld\n", (cfg_rec.errors)->ln, (cfg_rec.errors)->strerr,(cfg_rec.errors)->paramerr);
    printf("%d, %d, %ld\n", (cfg_rec.errors + 1)->ln, (cfg_rec.errors + 1)->strerr,(cfg_rec.errors + 1)->paramerr);
    /* int len, count; */
    /* len = getstring(fd, buffer, 4096, &status); */
    /* len = getstring(fd, buffer, 4096, &status); */
    /* len = getstring(fd, buffer, 4096, &status); */
    /* if (is_line_ending_ok(buffer, len)) */
    /*     fputs("<CR><LF> OK\n", stdout); */
    /* else */
    /*     fputs("NO <CR>\n", stdout); */
    /*  */
    /* count = get_param_count(buffer, len); */
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
