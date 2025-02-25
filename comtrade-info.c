#include <stdio.h>
#include <stdlib.h>

#include "strings.h"
#include "format.h"

#define STR_BUFSIZE             4096

#define EXIT_MEMERR()           exit(5)

#define IS_CORRECT_INTPARAM_VAL(a, b, c) is_correct_param_length(a, b, c)

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
            (p+i)->strerr |= ERRCODE(strcode);
            (p+i)->paramerr |= ERRCODE(paramcode);
            return;
        }
    }

    add_error_field(cfg_rec);
    (cfg_rec->errors+cfg_rec->errcount-1)->ln = line;
    (cfg_rec->errors+cfg_rec->errcount-1)->strerr |= ERRCODE(strcode);
    (cfg_rec->errors+cfg_rec->errcount-1)->paramerr |= ERRCODE(paramcode);
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
    char c[REVYEAR_LEN_MAX + 1];

    get_all_param_index(cfg_str->str, cfg_str->param_count, index);
    get_all_param_len(cfg_str->str, cfg_str->strlen, cfg_str->param_count, len);

    if (cfg_str->param_count < CP_HEADER_MIN) {
        add_error_code(cfg_str->nstr, LN_ERR_TOO_FEW_PARAM, 0, cfg_rec); 
        return 1;
    }
    if (!is_correct_param_length(len[0], SNAME_LEN_MIN, SNAME_LEN_MAX))
        add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM_LEN,
                PM_ERR_SNAME, cfg_rec);

    if (!is_correct_param_length(len[1], RECDEV_LEN_MIN, RECDEV_LEN_MAX))
        add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM_LEN,
                PM_ERR_REC_ID, cfg_rec);

    cfg_rec->station_name = add_str_item(cfg_str->str+index[0], len[0]);
    cfg_rec->rec_dev_id = add_str_item(cfg_str->str+index[1], len[1]);

    if (cfg_str->param_count < CP_HEADER) {
        cfg_rec->rev_year = 1991;
    } else {
        if (!is_correct_param_length(len[2], REVYEAR_LEN_MIN, REVYEAR_LEN_MAX)) {
            add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM_LEN,
                    PM_ERR_YEAR, cfg_rec);
            cfg_rec->rev_year = 1991;
        } else {
            stringcopy_c(c, cfg_str->str + index[2], len[2]);
            cfg_rec->rev_year = atoi(c);
            if (!IS_CORRECT_INTPARAM_VAL(cfg_rec->rev_year, REV_YEAR_VAL_MIN,
                        REV_YEAR_VAL_MAX))
                add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM,
                        PM_ERR_YEAR, cfg_rec);
        }
    }
    return 0;
}

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
        cfg_str.param_count = get_param_count(buffer, cfg_str.strlen) + 1;
        switch (next_state) {
        case analyze_header:
            analyze_cfg_header(&cfg_str, cfg_rec);
            next_state++;
            break;
        default:
            break;
        }
    }

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
    
    printf("Station name: %s\n", cfg_rec.station_name);
    printf("Recorder id: %s\n", cfg_rec.rec_dev_id);
    printf("Format revision: %d\n", cfg_rec.rev_year);

    fclose(fd);
    return 0;
} 
