#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strutils.h"
#include "format.h"

#define STR_BUFSIZE                         4096

#define EXIT_MEMERR()                       exit(5)

#define IS_CORRECT_INTPARAM_VAL(a, b, c)    is_correct_param_length(a, b, c)

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

typedef struct
{
    enum param_type ptype;
    int num;
    int len_min;
    int len_max;
    int ival_min;
    int ival_max;
    double dval_min;
    double dval_max;
} cfg_pvv_t;

const char lnerrmsg0[] = "Missing symbol <CR> at end of line";
const char lnerrmsg1[] = "Line contains extra spaces";
const char lnerrmsg2[] = "Line contains too many parametrs";
const char lnerrmsg3[] = "Line contains too few parametrs";
const char lnerrmsg4[] = "Incorrect parameter";
const char lnerrmsg5[] = "Incorrect parameter length";
const char lnerrmsg6[] = "Incorrect parameters summ";

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
        lnerrmsg4, lnerrmsg5, lnerrmsg6 };

const char *parammsg[] = { parammsg0, parammsg1, parammsg2, parammsg3,
        parammsg4, parammsg5, parammsg6, parammsg7, parammsg8, parammsg9,
        parammsg10, parammsg11, parammsg12, parammsg13, parammsg14, parammsg15,
        parammsg16, parammsg17, parammsg18, parammsg19, parammsg20, parammsg21,
        parammsg22, parammsg23, parammsg24, parammsg25, parammsg26, parammsg27,
        parammsg28, parammsg29, parammsg30, parammsg31, parammsg32, parammsg33, 
        parammsg34, parammsg35, parammsg36 }; 

const cfg_pvv_t sname = {pstring, PM_SNAME, SNAME_LEN_MIN, SNAME_LEN_MAX,
    0, 0, 0, 0};
const cfg_pvv_t recdevid = {pstring, PM_REC_ID, RECDEV_LEN_MIN, RECDEV_LEN_MAX,
    0, 0, 0, 0};
const cfg_pvv_t revyear = {pint, PM_YEAR, REVYEAR_LEN_MIN, REVYEAR_LEN_MAX,
    REV_YEAR_VAL_MIN, REV_YEAR_VAL_MAX, 0, 0};
const cfg_pvv_t tt = {pint, PM_TT, TT_LEN_MIN, TT_LEN_MAX,
    TT_VAL_MIN, TT_VAL_MAX, 0, 0};
const cfg_pvv_t tt_a = {pintc, PM_TT_A, AN_LEN_MIN, AN_LEN_MAX,
    AN_VAL_MIN, AN_VAL_MAX, 0, 0};
const cfg_pvv_t tt_d = {pintc, PM_TT_D, DN_LEN_MIN, DN_LEN_MAX,
    DN_VAL_MIN, DN_VAL_MAX, 0, 0};
const cfg_pvv_t ach_num = {pint, PM_AN, A_N_LEN_MIN, A_N_LEN_MAX,
    A_N_VAL_MIN, A_N_VAL_MAX, 0, 0};
const cfg_pvv_t ach_chid = {pstring, PM_CHID, A_CHID_LEN_MIN, A_CHID_LEN_MAX,
    0, 0, 0, 0};
const cfg_pvv_t ach_phase = {pstring, PM_PH, A_PH_LEN_MIN, A_PH_LEN_MAX,
    0, 0, 0, 0};
const cfg_pvv_t ach_ccbm = {pstring, PM_CCBM, A_CCBM_LEN_MIN, A_CCBM_LEN_MAX,
    0, 0, 0, 0};
const cfg_pvv_t ach_uu = {pstring, PM_UU, A_UU_LEN_MIN, A_UU_LEN_MAX,
    0, 0, 0, 0};

int match_char(char ch, char patt)
{
    if (ch == patt)
        return 1;
    else
        return 0;
}

int getstring(FILE *fd, char *buffer, int bufsize,
                enum getstring_status *status)
{
    int len = 0;

    *status = gss_ok;
    if (fgets(buffer, bufsize, fd)) {
        len = strlen(buffer);
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

int get_param_length(const char *str, int stringlen, int param, int param_count)
{
    int index;

    index = get_param_index(str, param);
    if ((param + 1) == param_count) {
        if (match_char(*(str+(stringlen - 2)), '\r'))
            return stringlen - 2 - index;
        else
            return stringlen - 1 - index;
    }
    else
        return get_param_index(str, param + 1) - index - 1;
}

#if 0
void get_all_param_len(const char *str, int stringlen, int param_count, int *len)
{
    int i;

    for (i = 0; i < param_count; i++)
        len[i] = get_param_length(str, stringlen, i, param_count);
}

void get_all_param_index(const char *str, int param_count, int *index)
{
    int i;

    for (i = 0; i < param_count; i++)
        index[i] = get_param_index(str, i);
}
#endif

void add_error_field(cmtrd_cfg_body_t *cfg_rec)
{
    cmtrd_err_t *p;

    p = malloc((cfg_rec->errcount + 1) * sizeof(cmtrd_err_t));
    if (p == NULL)
        EXIT_MEMERR();

    memcpy(p, cfg_rec->errors, cfg_rec->errcount * sizeof(cmtrd_err_t));
    (p+cfg_rec->errcount)->ln = 0;
    (p+cfg_rec->errcount)->strerr = 0;
    (p+cfg_rec->errcount)->paramerr = 0;
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
            if (paramcode != ERRNULL)
                (p+i)->paramerr |= paramcode;
            return;
        }
    }

    add_error_field(cfg_rec);
    (cfg_rec->errors+cfg_rec->errcount-1)->ln = line;
    (cfg_rec->errors+cfg_rec->errcount-1)->strerr |= strcode;
    if (paramcode != ERRNULL)
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

    str = malloc(sizeof(char) * length + 1);
    if (str == NULL)
        EXIT_MEMERR();

    stringcopy_c(str, src, length);
    return str;
}

void create_channels_fields(cmtrd_cfg_body_t *cfg_rec)
{
    void *p;

    if (cfg_rec->an_count) {
        p = malloc(sizeof(cmtrd_an_t) * cfg_rec->an_count);
        if (p == NULL)
            EXIT_MEMERR();
        memset(p, 0, sizeof(cmtrd_an_t));
        cfg_rec->anv = (cmtrd_an_t*)p;
    }
    if (cfg_rec->dn_count) {
        p = malloc(sizeof(cmtrd_dn_t) * cfg_rec->dn_count);
        if (p == NULL)
            EXIT_MEMERR();
        memset(p, 0, sizeof(cmtrd_dn_t));
        cfg_rec->dnv = (cmtrd_dn_t*)p;
    }
}

int check_param_count(int count, int min, int max)
{
    if (count < min)
        return ERRCODE(LN_ERR_TOO_FEW_PARAM);
    if (count > max)
        return ERRCODE(LN_ERR_TOO_MANY_PARAM);
    return 0;
}

void check_parameter_len(cfg_param_t *param)
{
    if (!is_correct_param_length(param->len, param->len_min, param->len_max))
        param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM_LEN);
}

void check_parameter_ival(cfg_param_t *param)
{
    if (!IS_CORRECT_INTPARAM_VAL(param->val_int, param->ival_min,
            param->ival_max))
    param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM);
}

void parsing_parameter(cfgfile_string_t *cfg_str, cfg_param_t *param)
{
    char s[PARAM_LEN_MAX+1];

    param->err = 0;
    param->index = get_param_index(cfg_str->str, param->num);
    param->len = get_param_length(cfg_str->str, cfg_str->strlen,
            param->num, cfg_str->param_count);
    check_parameter_len(param);
    switch (param->ptype) {
    case pstring:
    case pchar:
        break;
    case pint:
        stringcopy_c(s, cfg_str->str + param->index, param->len);
        param->val_int = atoi(s);
        check_parameter_ival(param);
        break;
    case pintc:
        stringcopy_c(s, cfg_str->str + param->index, param->len - 1);
        param->val_int = atoi(s);
        check_parameter_ival(param);
        break;
    case pfloat:
        break;
    default:
        break;
    }
}

int get_empty_anfield(cmtrd_cfg_body_t *cfg_rec, cmtrd_an_t *ch)
{
    int i;
    for (i = 0; i < cfg_rec->an_count; i++) {
        if ((ch + i)->num == 0)
            return i;
    }
    return -1;
}

int analyze_cfg_header(cfgfile_string_t *cfg_str, cmtrd_cfg_body_t *cfg_rec)
{
    int error;
    cfg_param_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_HEADER_MIN, CP_HEADER);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return 1;

    /* Station name */
    memcpy(&pm, &sname, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_SNAME),
                    cfg_rec); 
    cfg_rec->station_name = add_str_item(cfg_str->str, pm.len);

    /* Recorder device id */
    memcpy(&pm, &recdevid, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_REC_ID),
                    cfg_rec); 
    cfg_rec->rec_dev_id = add_str_item(cfg_str->str + pm.index, pm.len);

    if (cfg_str->param_count < CP_HEADER)
        return 0;

    /* Revison year */
    memcpy(&pm, &revyear, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_YEAR), cfg_rec); 
    cfg_rec->rev_year = pm.val_int;

    return 0;
}

int analyze_cfg_chinfo(cfgfile_string_t *cfg_str, cmtrd_cfg_body_t *cfg_rec)
{
    int error;
    cfg_param_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_TT, CP_TT);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return 1;

    error = 0;

    /* Total channels count */
    memcpy(&pm, &tt, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_TT), cfg_rec); 
    cfg_rec->ch_count = pm.val_int;

    /* Analog channels count */
    memcpy(&pm, &tt_a, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_TT_A), cfg_rec); 
    cfg_rec->an_count = pm.val_int;
    if (pm.err & ERRCODE(LN_ERR_INCORRECT_PARAM))
        error++;

    /* Digital channels count */
    memcpy(&pm, &tt_d, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_TT_D), cfg_rec); 
    cfg_rec->dn_count = pm.val_int;
    if (pm.err & ERRCODE(LN_ERR_INCORRECT_PARAM))
        error++;

    if ((cfg_rec->an_count + cfg_rec->dn_count) != cfg_rec->ch_count) {
        add_error_code(cfg_str->nstr, ERRCODE(LN_ERR_INCORRECT_PARAM_SUM),
                (ERRCODE(PM_ERR_TT) | ERRCODE(PM_ERR_TT_A) |
                 ERRCODE(PM_ERR_TT_D)), cfg_rec);
        error++;
    }

    return error;
}

int analyze_cfg_achannel(cfgfile_string_t *cfg_str, cmtrd_cfg_body_t *cfg_rec)
{
    int error;
    int ch_index;
    cfg_param_t pm;

    error = check_param_count(cfg_str->param_count, CP_AN_1991, CP_AN_1999);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return 1;

    ch_index = get_empty_anfield(cfg_rec, cfg_rec->anv);
    if (ch_index == -1)
        return ch_index;

    error = 0;

    /* Channel number */
    memcpy(&pm, &ach_num, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_AN), cfg_rec); 
    (cfg_rec->anv + ch_index)->num = pm.val_int;

    /* Channel id */
    memcpy(&pm, &ach_chid, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_CHID), cfg_rec); 
    (cfg_rec->anv + ch_index)->ch_id = add_str_item(cfg_str->str + pm.index,
            pm.len);

    /* Channel phase */
    memcpy(&pm, &ach_phase, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_PH), cfg_rec); 
    (cfg_rec->anv + ch_index)->phase = add_str_item(cfg_str->str + pm.index,
            pm.len);

    /* Channel circuit component */
    memcpy(&pm, &ach_ccbm, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_CCBM), cfg_rec); 
    (cfg_rec->anv + ch_index)->ccbm = add_str_item(cfg_str->str + pm.index,
            pm.len);

    /* Channel unit */
    memcpy(&pm, &ach_uu, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, &pm);
    if (pm.err)
        add_error_code(cfg_str->nstr, pm.err, ERRCODE(PM_ERR_UU), cfg_rec); 
    (cfg_rec->anv + ch_index)->uu = add_str_item(cfg_str->str + pm.index,
            pm.len);

    return error;
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
            add_error_code(cfg_str.nstr, ERRCODE(LN_ERR_NOCR), ERRNULL, cfg_rec);

        cfg_str.str = buffer;
        cfg_str.param_count = get_param_count(buffer, cfg_str.strlen) + 1;
        switch (next_state) {
        case analyze_header:
            analyze_cfg_header(&cfg_str, cfg_rec);
            next_state++;
            break;
        case analyze_tt:
            if (analyze_cfg_chinfo(&cfg_str, cfg_rec))
                return 1;
            create_channels_fields(cfg_rec);
            next_state++;
            break;
        case analyze_ach:
            analyze_cfg_achannel(&cfg_str, cfg_rec);
            next_state++;
            break;
        default:
            return 0;
            break;
        }
        cfg_str.nstr++;
    }

    return 0;
}

void cfg_record_init(cmtrd_cfg_body_t *cfg_rec)
{
    memset(cfg_rec, 0, sizeof(cmtrd_cfg_body_t));
    cfg_rec->rev_year = 1991;
}

void print_errors(cmtrd_cfg_body_t *cfg_rec)
{
    int i;

    printf("\nThe following errors were found:\n");
    for (i = 0; i < cfg_rec->errcount; i++) {
        int msg_index;

        printf("line %d:\n", (cfg_rec->errors + i)->ln + 1);
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
void print_info(cmtrd_cfg_body_t *cfg_rec)
{
    printf("General info:\n");
    printf("    Station name: %s\n", cfg_rec->station_name);
    printf("    Recorder id: %s\n", cfg_rec->rec_dev_id);
    printf("    Format revision: %d\n", cfg_rec->rev_year);
    printf("Channels info:\n");
    printf("    Total channels count: %d\n", cfg_rec->ch_count);
    printf("    Analog channels count: %d\n", cfg_rec->an_count);
    printf("    Digital channels count: %d\n", cfg_rec->dn_count);
    printf("Analog channel:\n");
    printf("    Channel number: %d\n", cfg_rec->anv->num);
    printf("    Channel id: %s\n", cfg_rec->anv->ch_id);
    printf("    Channel phase: %s\n", cfg_rec->anv->phase);
    printf("    Channel circuit: %s\n", cfg_rec->anv->ccbm);
    printf("    Channel unit: %s\n", cfg_rec->anv->uu);
}

/* return codes:
 * 0 - Ok
 * 1 - file read error
*/
int main(int argc, char **argv)
{
    int res;
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
    res = analyze_cfgfile(fd, &cfg_rec);
    print_info(&cfg_rec);
    if (cfg_rec.errors)
        print_errors(&cfg_rec);

    fclose(fd);
    if (res) {
        printf("Critical error. Analysis has been aborted.\n");
        return 2;
    }
    return 0;
} 
