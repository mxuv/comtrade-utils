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

const char lnerrmsg0[] = "Missing symbol <CR> at end of line";
const char lnerrmsg1[] = "Line contains extra spaces";
const char lnerrmsg2[] = "Line contains too many parametrs";
const char lnerrmsg3[] = "Line contains too few parametrs";
const char lnerrmsg4[] = "Incorrect parameter";
const char lnerrmsg5[] = "Incorrect parameter length";

const char parammsg0[] = "Station name";
const char parammsg1[] = "Recording device id";
const char parammsg2[] = "Year of the standart revision";
const char parammsg3[] = "Total channels count";
const char parammsg4[] = "Analog channels count";
const char parammsg5[] = "Digital channels count";
const char parammsg6[] = "Analog channel number";
const char parammsg7[] = "Channel id";
const char parammsg8[] = "Phase id";
const char parammsg9[] = "Circuit component";
const char parammsg10[] = "Channel unit";
const char parammsg11[] = "Channel multipler";
const char parammsg12[] = "Channel offset";
const char parammsg13[] = "Channel time skew";
const char parammsg14[] = "Channel minimum data value";
const char parammsg15[] = "Channel maximum data value";
const char parammsg16[] = "Channel transformer ratio primary factor";
const char parammsg17[] = "Channel transformer ratio secondary factor";
const char parammsg18[] = "Channel primary or secondary data scalling indentifier;";
const char parammsg19[] = "Digital channel number";
const char parammsg20[] = "Channel normal state";
const char parammsg21[] = "Line frequency";
const char parammsg22[] = "Number of sampling rates";
const char parammsg23[] = "Sample rate";
const char parammsg24[] = "Last sample number";
const char parammsg25[] = "Day";
const char parammsg26[] = "Month";
const char parammsg27[] = "Year";
const char parammsg28[] = "Hour";
const char parammsg29[] = "Minuts";
const char parammsg30[] = "Seconds";
const char parammsg31[] = "Data file type";
const char parammsg32[] = "Time multiplication factor";
const char parammsg33[] = "Time code";
const char parammsg34[] = "Local time code";
const char parammsg35[] = "Time quality code";
const char parammsg36[] = "Leap second indicator";

const char *lnerrmsg[] = { lnerrmsg0, lnerrmsg1, lnerrmsg2, lnerrmsg3,
        lnerrmsg4, lnerrmsg5 };

const char *parammsg[] = { parammsg0, parammsg1, parammsg2, parammsg3,
        parammsg4, parammsg5, parammsg6, parammsg7, parammsg8, parammsg9,
        parammsg10, parammsg11, parammsg12, parammsg13, parammsg14, parammsg15,
        parammsg16, parammsg17, parammsg18, parammsg19, parammsg20, parammsg21,
        parammsg22, parammsg23, parammsg24, parammsg25, parammsg26, parammsg27,
        parammsg28, parammsg29, parammsg30, parammsg31, parammsg32, parammsg33, 
        parammsg34, parammsg35, parammsg36 }; 

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
            if (paramcode != ERRNULL)
                (p+i)->paramerr |= ERRCODE(paramcode);
            return;
        }
    }

    add_error_field(cfg_rec);
    (cfg_rec->errors+cfg_rec->errcount-1)->ln = line;
    (cfg_rec->errors+cfg_rec->errcount-1)->strerr |= ERRCODE(strcode);
    if (paramcode != ERRNULL)
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
    int len[SNAME_LEN_MAX];
    char c[REVYEAR_LEN_MAX + 1];

    cfg_rec->rev_year = 1991;
    if (cfg_str->param_count < CP_HEADER_MIN) {
        add_error_code(cfg_str->nstr, LN_ERR_TOO_FEW_PARAM, ERRNULL, cfg_rec); 
        return 1;
    }
    if (cfg_str->param_count > CP_HEADER) {
        add_error_code(cfg_str->nstr, LN_ERR_TOO_MANY_PARAM, ERRNULL, cfg_rec); 
        return 1;
    }

    get_all_param_index(cfg_str->str, cfg_str->param_count, index);
    get_all_param_len(cfg_str->str, cfg_str->strlen, cfg_str->param_count, len);
    if (!is_correct_param_length(len[0], SNAME_LEN_MIN, SNAME_LEN_MAX))
        add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM_LEN,
                PM_ERR_SNAME, cfg_rec);

    if (!is_correct_param_length(len[1], RECDEV_LEN_MIN, RECDEV_LEN_MAX))
        add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM_LEN,
                PM_ERR_REC_ID, cfg_rec);

    cfg_rec->station_name = add_str_item(cfg_str->str+index[0], len[0]);
    cfg_rec->rec_dev_id = add_str_item(cfg_str->str+index[1], len[1]);

    if (cfg_str->param_count == CP_HEADER) {
        if (!is_correct_param_length(len[2], REVYEAR_LEN_MIN, REVYEAR_LEN_MAX)) {
            add_error_code(cfg_str->nstr, LN_ERR_INCORRECT_PARAM_LEN,
                    PM_ERR_YEAR, cfg_rec);
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
            add_error_code(cfg_str.nstr, LN_ERR_NOCR, ERRNULL, cfg_rec);

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

void print_errors(cmtrd_cfg_body_t *cfg_rec)
{
    int i;

    printf("\nThe following errors were found:\n");
    for (i = 0; i < cfg_rec->errcount; i++) {
        int msg_index;

        printf("line %d:\n", (cfg_rec->errors + i)->ln);
        printf("    line errors:\n");
        for (msg_index = 0; msg_index < LN_ERR_COUNT; msg_index++) {
            int j = 1;
            if ((cfg_rec->errors + i)->strerr & (j << msg_index))
                printf("        %s\n", lnerrmsg[msg_index]);
        }
        if ((cfg_rec->errors + i)->paramerr) {
            long int j = 1;
            printf("    parameters with errors:\n");
            for (msg_index = 0; msg_index < PARAM_COUNT_MAX; msg_index++) {
                if ((cfg_rec->errors + i)->paramerr & (j << msg_index))
                    printf("        %s\n", parammsg[msg_index]);
            }
        }
    }
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
    
    printf("Station name: %s\n", cfg_rec.station_name);
    printf("Recorder id: %s\n", cfg_rec.rec_dev_id);
    printf("Format revision: %d\n", cfg_rec.rev_year);

    if (cfg_rec.errors)
        print_errors(&cfg_rec);

    fclose(fd);
    return 0;
} 
