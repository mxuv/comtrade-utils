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
} cfg_str_t;

typedef struct
{
    enum param_type ptype;
    int num;
    int err;
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
const char lnerrmsg7[] = "Format line dosen't match revision";

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
    lnerrmsg4, lnerrmsg5, lnerrmsg6, lnerrmsg7 };

const char *parammsg[] = { parammsg0, parammsg1, parammsg2, parammsg3,
    parammsg4, parammsg5, parammsg6, parammsg7, parammsg8, parammsg9,
    parammsg10, parammsg11, parammsg12, parammsg13, parammsg14, parammsg15,
    parammsg16, parammsg17, parammsg18, parammsg19, parammsg20, parammsg21,
    parammsg22, parammsg23, parammsg24, parammsg25, parammsg26, parammsg27,
    parammsg28, parammsg29, parammsg30, parammsg31, parammsg32, parammsg33, 
    parammsg34, parammsg35, parammsg36 }; 

const cfg_pvv_t sname = {
    pstring,
    PM_SNAME_POS,
    PM_ERR_SNAME,
    SNAME_LEN_MIN,
    SNAME_LEN_MAX, 
    0,
    0,
    0,
    0
};
const cfg_pvv_t recdevid = {
    pstring,
    PM_REC_ID_POS,
    PM_ERR_REC_ID,
    RECDEV_LEN_MIN,
    RECDEV_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t revyear = {
    pint,
    PM_YEAR_POS,
    PM_ERR_YEAR,
    REVYEAR_LEN_MIN,
    REVYEAR_LEN_MAX,
    REV_YEAR_VAL_MIN,
    REV_YEAR_VAL_MAX,
    0,
    0
};
const cfg_pvv_t tt = {
    pint,
    PM_TT_POS,
    PM_ERR_TT,
    TT_LEN_MIN,
    TT_LEN_MAX,
    TT_VAL_MIN,
    TT_VAL_MAX,
    0,
    0
};
const cfg_pvv_t tt_a = {
    pintc,
    PM_TT_A_POS,
    PM_ERR_TT_A,
    AN_LEN_MIN,
    AN_LEN_MAX,
    AN_VAL_MIN,
    AN_VAL_MAX,
    0,
    0
};
const cfg_pvv_t tt_d = {
    pintc,
    PM_TT_D_POS,
    PM_ERR_TT_D,
    DN_LEN_MIN,
    DN_LEN_MAX,
    DN_VAL_MIN,
    DN_VAL_MAX,
    0,
    0
};
const cfg_pvv_t ach_num = {
    pint,
    PM_AN_POS,
    PM_ERR_AN,
    A_N_LEN_MIN,
    A_N_LEN_MAX,
    A_N_VAL_MIN,
    A_N_VAL_MAX,
    0,
    0
};
const cfg_pvv_t ach_chid = {
    pstring,
    PM_CHID_POS,
    PM_ERR_CHID,
    A_CHID_LEN_MIN,
    A_CHID_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_phase = {
    pstring,
    PM_PH_POS,
    PM_ERR_PH,
    A_PH_LEN_MIN,
    A_PH_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_ccbm = {
    pstring,
    PM_CCBM_POS,
    PM_ERR_CCBM,
    A_CCBM_LEN_MIN,
    A_CCBM_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_uu = {
    pstring, 
    PM_UU_POS,
    PM_ERR_UU,
    A_UU_LEN_MIN,
    A_UU_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_a = {
    pfloat,
    PM_A_POS,
    PM_ERR_A,
    A_A_LEN_MIN,
    A_A_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_b = {
    pfloat,
    PM_B_POS,
    PM_ERR_B,
    A_B_LEN_MIN,
    A_B_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_skew = {
    pfloat,
    PM_SKEW_POS,
    PM_ERR_SKEW,
    A_SKEW_LEN_MIN,
    A_SKEW_LEN_MAX,
    0,
    0,
    0,
    0
}; const cfg_pvv_t ach_min = {
    pfloat,
    PM_MIN_POS,
    PM_ERR_MIN,
    A_MINSC_LEN_MIN,
    A_MINSC_LEN_MAX,
    0,
    0,
    A_MINSC_VAL_MIN,
    A_MINSC_VAL_MAX
};
const cfg_pvv_t ach_max = {
    pfloat,
    PM_MAX_POS,
    PM_ERR_MAX,
    A_MAXSC_LEN_MIN,
    A_MAXSC_LEN_MAX,
    0,
    0,
    A_MAXSC_VAL_MIN,
    A_MAXSC_VAL_MAX
};
const cfg_pvv_t ach_primary = {
    pfloat,
    PM_PRIM_POS,
    PM_ERR_PRIM,
    A_PRIMARY_LEN_MIN,
    A_PRIMARY_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_secondary = {
    pfloat,
    PM_SEC_POS,
    PM_ERR_SEC,
    A_SECONDARY_LEN_MIN,
    A_SECONDARY_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t ach_ps = {
    pchar,
    PM_PS_POS,
    PM_ERR_PS,
    A_PS_LEN_MIN,
    A_PS_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t dch_num = {
    pint,
    PM_DN_POS,
    PM_ERR_DN,
    D_N_LEN_MIN,
    D_N_LEN_MAX,
    D_N_VAL_MIN,
    D_N_VAL_MAX,
    0,
    0
};
const cfg_pvv_t dch_chid = {
    pstring,
    PM_CHID_POS,
    PM_ERR_CHID,
    D_CHID_LEN_MIN,
    D_CHID_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t dch_phase = {
    pstring,
    PM_PH_POS,
    PM_ERR_PH,
    D_PH_LEN_MIN,
    D_PH_LEN_MAX,
    0, 
    0,
    0,
    0
};
const cfg_pvv_t dch_ccbm = {
    pstring,
    PM_CCBM_POS,
    PM_ERR_CCBM,
    D_CCBM_LEN_MIN,
    D_CCBM_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t dch_y1991 = {
    pint,
    PM_Y_1991_POS,
    PM_ERR_Y,
    D_Y_LEN_MIN,
    D_Y_LEN_MAX,
    D_Y_VAL_MIN,
    D_Y_VAL_MAX,
    0,
    0
};
const cfg_pvv_t dch_y1999 = {
    pint,
    PM_Y_1999_POS,
    PM_ERR_Y,
    D_Y_LEN_MIN,
    D_Y_LEN_MAX,
    D_Y_VAL_MIN,
    D_Y_VAL_MAX,
    0,
    0
};

const cfg_pvv_t *pvv[] = { &sname, &recdevid, &revyear, &tt, &tt_a, &tt_d,
    &ach_num, &ach_chid, &ach_phase, &ach_ccbm, &ach_uu, &ach_a, &ach_b,
    &ach_skew, &ach_min, &ach_max, &ach_primary, &ach_secondary, &ach_ps,
    &dch_num, &dch_chid, &dch_phase, &dch_ccbm, &dch_y1991, &dch_y1999 };

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

void add_error_field(cmtrd_cfg_t *cfg_rec)
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
                    cmtrd_cfg_t *cfg_rec)
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

void create_channels_fields(cmtrd_cfg_t *cfg_rec)
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

void check_parameter_len(cfg_pm_t *param, int min, int max)
{
    if (!is_correct_param_length(param->len, min, max))
        param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM_LEN);
}

void check_parameter_ival(cfg_pm_t *param, int min, int max)
{
    if (!IS_CORRECT_INTPARAM_VAL(param->val_int, min, max))
        param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM);
}

int is_correct_dparam_val(double val, double min, double max)
{
    if (min == 0 && max == 0)
        return 1;

    if (val > min && val < max)
        return 1;
    else
        return 0;
}

void check_parameter_dval(cfg_pm_t *param, double min, double max)
{
    if (!is_correct_dparam_val(param->val_float, min, max))
    param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM);
}

char check_ps_value(cfg_str_t *cfg_str, cfg_pm_t *pm)
{
    char c;

    c = cfg_str->str[pm->index];
    c = upcase_letter(c);
    if (c == 'P' || c == 'S') {
        return c;
    } else {
        pm->err |= ERRCODE(LN_ERR_INCORRECT_PARAM);
        return '-';
    }
}

int get_empty_field(int *p, int count, int fieldsize)
{
    int i;
    for (i = 0; i < count; i++) {
        if (*(p + (i * fieldsize / sizeof(*p))) == 0)
            return i;
    }
    return -1;
}

#if 0
int get_empty_anfield(cmtrd_cfg_t *cfg_rec, cmtrd_an_t *ch)
{
    int i;
    for (i = 0; i < cfg_rec->an_count; i++) {
        if ((ch + i)->num == 0)
            return i;
    }
    return -1;
}

int get_empty_dnfield(cmtrd_cfg_t *cfg_rec, cmtrd_dn_t *ch)
{
    int i;
    for (i = 0; i < cfg_rec->dn_count; i++) {
        if ((ch + i)->num == 0)
            return i;
    }
    return -1;
}
#endif

int is_match_ach_rev(int param_count, int rev_year)
{
    if (rev_year >= rev1991 && rev_year < rev1999 && param_count == CP_AN_1991)
        return 1;

    if (rev_year >= rev1999 && param_count == CP_AN_1999)
        return 1;

    return 0;
}

int is_match_dch_rev(int param_count, int rev_year)
{
    if (rev_year >= rev1991 && rev_year < rev1999 && param_count == CP_DN_1991)
        return 1;

    if (rev_year >= rev1999 && param_count == CP_DN_1999)
        return 1;

    return 0;
}

void save_header_items(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    switch (pn) {
    case psname:
        cfg_rec->station_name = add_str_item(cfg_str->str + pm->index, pm->len);
        break;
    case prec_id:
        cfg_rec->rec_dev_id = add_str_item(cfg_str->str + pm->index, pm->len);
        break;
    case prev_year:
        cfg_rec->rev_year = pm->val_int;
        break;
    default:
        break;
    }
}

void save_chinfo_items(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    switch (pn) {
    case ptt:
        cfg_rec->ch_count = pm->val_int;
        break;
    case ptt_a:
        cfg_rec->an_count = pm->val_int;
        break;
    case ptt_d:
        cfg_rec->dn_count = pm->val_int;
        break;
    default:
        break;
    }
}

void save_achannel_items(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    switch (pn) {
    case pan:
        (cfg_rec->anv + pm->ch_index)->num = pm->val_int;
        break;
    case pach_id:
        (cfg_rec->anv + pm->ch_index)->ch_id = add_str_item(cfg_str->str +
                pm->index, pm->len);
        break;
    case paphase:
        (cfg_rec->anv + pm->ch_index)->phase = add_str_item(cfg_str->str +
                pm->index, pm->len);
        break;
    case paccbm:
        (cfg_rec->anv + pm->ch_index)->ccbm = add_str_item(cfg_str->str +
                pm->index, pm->len);
        break;
    case pauu:
        (cfg_rec->anv + pm->ch_index)->uu = add_str_item(cfg_str->str +
                pm->index, pm->len);
        break;
    case paa:
        (cfg_rec->anv + pm->ch_index)->a = pm->val_float;
        break;
    case pab:
        (cfg_rec->anv + pm->ch_index)->b = pm->val_float;
        break;
    case paskew:
        (cfg_rec->anv + pm->ch_index)->skew = pm->val_float;
        break;
    case pamin:
        (cfg_rec->anv + pm->ch_index)->min = pm->val_float;
        break;
    case pamax:
        (cfg_rec->anv + pm->ch_index)->max = pm->val_float;
        break;
    case paprimary:
        (cfg_rec->anv + pm->ch_index)->primary = pm->val_float;
        break;
    case pasecondary:
        (cfg_rec->anv + pm->ch_index)->secondary = pm->val_float;
        break;
    case paps:
        (cfg_rec->anv + pm->ch_index)->ps = check_ps_value(cfg_str, pm);
        break;
    default:
        break;
    }
}

void save_dchannel_items(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    switch (pn) {
    case pdn:
        (cfg_rec->dnv + pm->ch_index)->num = pm->val_int;
        break;
    case pdch_id:
        (cfg_rec->dnv + pm->ch_index)->ch_id = add_str_item(cfg_str->str +
                pm->index, pm->len);
        break;
    case pdphase:
        (cfg_rec->dnv + pm->ch_index)->phase = add_str_item(cfg_str->str +
                pm->index, pm->len);
        break;
    case pdccbm:
        (cfg_rec->dnv + pm->ch_index)->ccbm = add_str_item(cfg_str->str +
                pm->index, pm->len);
        break;
    case pdy_1991:
    case pdy_1999:
        (cfg_rec->dnv + pm->ch_index)->y = pm->val_int;
        break;
    default:
        break;
    }
}

void save_value2rec(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    if (pn >= psname && pn <= prev_year) {
        save_header_items(pn, cfg_str, pm, cfg_rec);
        return;
    }

    if (pn >= ptt && pn <= ptt_d) {
        save_chinfo_items(pn, cfg_str, pm, cfg_rec);
        return;
    }

    if (pn >= pan && pn <= paps) {
        save_achannel_items(pn, cfg_str, pm, cfg_rec);
        return;
    }

    if (pn >= pdn && pn <= pdy_1999) {
        save_dchannel_items(pn, cfg_str, pm, cfg_rec);
        return;
    }
    switch (pn) {
    default:
        break;
    }
}

void parsing_parameter(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    char s[PARAM_LEN_MAX+1];

    pm->err = 0;
    pm->index = get_param_index(cfg_str->str, pvv[pn]->num);
    pm->len = get_param_length(cfg_str->str, cfg_str->strlen, pvv[pn]->num,
            cfg_str->param_count);
    check_parameter_len(pm, pvv[pn]->len_min, pvv[pn]->len_max);
    switch (pvv[pn]->ptype) {
    case pstring:
    case pchar:
        break;
    case pint:
        stringcopy_c(s, cfg_str->str + pm->index, pm->len);
        pm->val_int = atoi(s);
        check_parameter_ival(pm, pvv[pn]->ival_min, pvv[pn]->ival_max);
        break;
    case pintc:
        stringcopy_c(s, cfg_str->str + pm->index, pm->len - 1);
        pm->val_int = atoi(s);
        check_parameter_ival(pm, pvv[pn]->ival_min, pvv[pn]->ival_max);
        break;
    case pfloat:
        stringcopy_c(s, cfg_str->str + pm->index, pm->len);
        pm->val_float = atof(s);
        check_parameter_dval(pm, pvv[pn]->dval_min, pvv[pn]->dval_max);
        break;
    default:
        break;
    }

    save_value2rec(pn, cfg_str, pm, cfg_rec);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(pvv[pn]->err), cfg_rec); 
}

#if 0
void parsing_recdevid(cfg_pm_t *pm, cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    memcpy(pm, &recdevid, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_REC_ID), cfg_rec); 
    cfg_rec->rec_dev_id = add_str_item(cfg_str->str + pm->index, pm->len);
}

void parsing_revyear(cfg_pm_t *pm, cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    memcpy(pm, &revyear, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_YEAR), cfg_rec); 
    cfg_rec->rev_year = pm->val_int;
}

void parsing_tt(cfg_pm_t *pm, cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    memcpy(pm, &tt, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_TT), cfg_rec); 
    cfg_rec->ch_count = pm->val_int;
}

void parsing_tt_a(cfg_pm_t *pm, cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    memcpy(pm, &tt_a, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_TT_A), cfg_rec); 
    cfg_rec->an_count = pm->val_int;
}

void parsing_tt_d(cfg_pm_t *pm, cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    memcpy(pm, &tt_d, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_TT_D), cfg_rec); 
    cfg_rec->dn_count = pm->val_int;
}

#endif
int analyze_cfg_header(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_HEADER_MIN,
            CP_HEADER_MAX);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return 1;

    /* Station name */
    parsing_parameter(psname, cfg_str, &pm, cfg_rec);

    /* Recorder device id */
    parsing_parameter(prec_id, cfg_str, &pm, cfg_rec);

    if (cfg_str->param_count < CP_HEADER_MAX)
        return 0;

    /* Revison year */
    parsing_parameter(prev_year, cfg_str, &pm, cfg_rec);

    return 0;
}

/* Total channels count */
/* Analog channels count */
/* Digital channels count */
int analyze_cfg_chinfo(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    enum cfg_pnum pn;
    
    error = check_param_count(cfg_str->param_count, CP_TT, CP_TT);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return 1;

    error = 0;
    for (pn = ptt; pn < pan; pn++) {
        parsing_parameter(pn, cfg_str, &pm, cfg_rec);
        if (pm.err & ERRCODE(LN_ERR_INCORRECT_PARAM))
            error++;
    }

    if ((cfg_rec->an_count + cfg_rec->dn_count) != cfg_rec->ch_count) {
        add_error_code(cfg_str->nstr, ERRCODE(LN_ERR_INCORRECT_PARAM_SUM),
                (ERRCODE(PM_ERR_TT) | ERRCODE(PM_ERR_TT_A) |
                 ERRCODE(PM_ERR_TT_D)), cfg_rec);
        error++;
    }
    return error;
}

#if 0
void parsing_a_num(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_num, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_AN), cfg_rec); 
    (cfg_rec->anv + ch_index)->num = pm->val_int;
}

void parsing_a_chid(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_chid, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_CHID), cfg_rec); 
    (cfg_rec->anv + ch_index)->ch_id = add_str_item(cfg_str->str + pm->index,
            pm->len);
}

void parsing_a_phase(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_phase, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_PH), cfg_rec); 
    (cfg_rec->anv + ch_index)->phase = add_str_item(cfg_str->str + pm->index,
            pm->len);
}

void parsing_a_ccbm(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_ccbm, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_CCBM), cfg_rec); 
    (cfg_rec->anv + ch_index)->ccbm = add_str_item(cfg_str->str + pm->index,
            pm->len);
}

void parsing_a_uu(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_uu, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_UU), cfg_rec); 
    (cfg_rec->anv + ch_index)->uu = add_str_item(cfg_str->str + pm->index,
            pm->len);
}

void parsing_a_a(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_a, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_A), cfg_rec); 
    (cfg_rec->anv + ch_index)->a = pm->val_float;
}

void parsing_a_b(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_b, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_B), cfg_rec); 
    (cfg_rec->anv + ch_index)->b = pm->val_float;
}

void parsing_a_skew(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_skew, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_SKEW), cfg_rec); 
    (cfg_rec->anv + ch_index)->skew = pm->val_float;
}

void parsing_a_min(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_min, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_MIN), cfg_rec); 
    (cfg_rec->anv + ch_index)->min = pm->val_float;
}

void parsing_a_max(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_max, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_MAX), cfg_rec); 
    (cfg_rec->anv + ch_index)->max = pm->val_float;
}

void parsing_a_prim(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_primary, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_PRIM), cfg_rec); 
    (cfg_rec->anv + ch_index)->primary = pm->val_float;
}

void parsing_a_sec(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_secondary, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_SEC), cfg_rec); 
    (cfg_rec->anv + ch_index)->secondary = pm->val_float;
}

void parsing_a_ps(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &ach_ps, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    (cfg_rec->anv + ch_index)->ps = check_ps_value(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_PS), cfg_rec);
}
#endif

    /* Channel number */
    /* Channel id */
    /* Channel phase */
    /* Channel circuit component */
    /* Channel unit */
    /* Channel multipler (a) */
    /* Channel offset (b) */
    /* Channel time skew */
    /* Channel minimum scale */
    /* Channel maximum scale */
    /* channel primary value */
    /* channel secondary value */
    /* channel p/s */
int analyze_cfg_achannel(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    enum cfg_pnum pn;

    error = check_param_count(cfg_str->param_count, CP_AN_1991, CP_AN_1999);
    pm.ch_index = get_empty_field(&cfg_rec->anv->num, cfg_rec->an_count,
            sizeof(cmtrd_an_t));
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return pm.ch_index;
    if (pm.ch_index == -1)
        return pm.ch_index;

    for (pn = pan; pn < paprimary; pn++)
        parsing_parameter(pn, cfg_str, &pm, cfg_rec);

    if (cfg_str->param_count != CP_AN_1999)
        return pm.ch_index;

    for (pn = paprimary; pn < pdn; pn++)
        parsing_parameter(pn, cfg_str, &pm, cfg_rec);

    return pm.ch_index;
}

#if 0
void parsing_d_num(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &dch_num, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_DN), cfg_rec); 
    (cfg_rec->dnv + ch_index)->num = pm->val_int;
}

void parsing_d_chid(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &dch_chid, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_CHID), cfg_rec); 
    (cfg_rec->dnv + ch_index)->ch_id = add_str_item(cfg_str->str + pm->index,
            pm->len);
}

void parsing_d_phase(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &dch_phase, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_PH), cfg_rec); 
    (cfg_rec->dnv + ch_index)->phase = add_str_item(cfg_str->str + pm->index,
            pm->len);
}

void parsing_d_ccbm(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    memcpy(pm, &dch_ccbm, sizeof(cfg_pvv_t));
    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_CCBM), cfg_rec); 
    (cfg_rec->dnv + ch_index)->ccbm = add_str_item(cfg_str->str + pm->index,
            pm->len);
}

void parsing_d_y(cfg_pm_t *pm, cfg_str_t *cfg_str,
        cmtrd_cfg_t *cfg_rec, int ch_index)
{
    if (cfg_rec->rev_year < rev1999)
        memcpy(pm, &dch_y1991, sizeof(cfg_pvv_t));
    else
        memcpy(pm, &dch_y1999, sizeof(cfg_pvv_t));

    parsing_parameter(cfg_str, pm);
    if (pm->err)
        add_error_code(cfg_str->nstr, pm->err, ERRCODE(PM_ERR_Y), cfg_rec); 
    (cfg_rec->dnv + ch_index)->y = pm->val_int;
}

#endif
int analyze_cfg_dchannel(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;

    error = check_param_count(cfg_str->param_count, CP_DN_1991, CP_DN_1999);
    pm.ch_index = get_empty_field(&cfg_rec->dnv->num, cfg_rec->dn_count,
            sizeof(cmtrd_dn_t));
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return pm.ch_index;
    if (pm.ch_index == -1)
        return pm.ch_index;

    /* Channel number */
    parsing_parameter(pdn, cfg_str, &pm, cfg_rec);

    /* Channel id */
    parsing_parameter(pdch_id, cfg_str, &pm, cfg_rec);

    if (cfg_str->param_count >= CP_DN_1999) {

        /* Channel phase */
        parsing_parameter(pdphase, cfg_str, &pm, cfg_rec);

        /* Channel circuit component */
        parsing_parameter(pdccbm, cfg_str, &pm, cfg_rec);

        /* Channel default state */
        parsing_parameter(pdy_1999, cfg_str, &pm, cfg_rec);
    }

    /* Channel default state */
    parsing_parameter(pdy_1991, cfg_str, &pm, cfg_rec);

    return pm.ch_index;
}

/* Return values:
 * 0-Ok
 * 2-Unexcepted end of file
 * 3-Read error
 * 4-Buffer overflow
*/
int analyze_cfgfile(FILE *fd, cmtrd_cfg_t *cfg_rec)
{
    int result;
    char buffer[STR_BUFSIZE];
    cfg_str_t cfg_str;
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
            result = analyze_cfg_achannel(&cfg_str, cfg_rec);
            if (result == -1 || result == cfg_rec->an_count - 1)
                next_state++;
            if (!is_match_ach_rev(cfg_str.param_count, cfg_rec->rev_year))
                add_error_code(cfg_str.nstr, ERRCODE(LN_ERR_MATCH_REV_YEAR),
                    ERRNULL, cfg_rec);
            break;
        case analyze_dch:
            result = analyze_cfg_dchannel(&cfg_str, cfg_rec);
            if (result == -1 || result == cfg_rec->dn_count - 1)
                next_state++;
            if (!is_match_dch_rev(cfg_str.param_count, cfg_rec->rev_year))
                add_error_code(cfg_str.nstr, ERRCODE(LN_ERR_MATCH_REV_YEAR),
                    ERRNULL, cfg_rec);
            break;
        default:
            return 0;
            break;
        }
        cfg_str.nstr++;
    }

    return 0;
}

void cfg_record_init(cmtrd_cfg_t *cfg_rec)
{
    memset(cfg_rec, 0, sizeof(cmtrd_cfg_t));
    cfg_rec->rev_year = 1991;
}

void print_errors(cmtrd_cfg_t *cfg_rec)
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

void print_achannels_info(cmtrd_cfg_t *cfg_rec)
{
    int i;

    for (i = 0; i < cfg_rec->an_count; i++) {
        printf("Analog channel:\n");
        printf("    Channel number: %d\n", (cfg_rec->anv + i)->num);
        printf("    Channel id: %s\n", (cfg_rec->anv + i)->ch_id);
        printf("    Channel phase: %s\n", (cfg_rec->anv + i)->phase);
        printf("    Channel circuit: %s\n", (cfg_rec->anv + i)->ccbm);
        printf("    Channel unit: %s\n", (cfg_rec->anv + i)->uu);
        printf("    Channel multipler: %lf\n", (cfg_rec->anv + i)->a);
        printf("    Channel offset: %lf\n", (cfg_rec->anv + i)->b);
        printf("    Channel time skew: %lf\n", (cfg_rec->anv + i)->skew);
        printf("    Channel min scale: %lf\n", (cfg_rec->anv + i)->min);
        printf("    Channel max scale: %lf\n", (cfg_rec->anv + i)->max);
        printf("    Channel primary value: %lf\n", (cfg_rec->anv + i)->primary);
        printf("    Channel secondary value: %lf\n",
                (cfg_rec->anv + i)->secondary);
        printf("    Channel P or S: %c\n", (cfg_rec->anv + i)->ps);

    }
}

void print_dchannels_info(cmtrd_cfg_t *cfg_rec)
{
    int i;

    for (i = 0; i < cfg_rec->dn_count; i++) {
        printf("Digital channel:\n");
        printf("    Channel number: %d\n", (cfg_rec->dnv + i)->num);
        printf("    Channel id: %s\n", (cfg_rec->dnv + i)->ch_id);
        printf("    Channel phase: %s\n", (cfg_rec->dnv + i)->phase);
        printf("    Channel circuit: %s\n", (cfg_rec->dnv + i)->ccbm);
        printf("    Channel default state: %d\n", (cfg_rec->dnv + i)->y);

    }
}

void print_info(cmtrd_cfg_t *cfg_rec)
{
    printf("General info:\n");
    printf("    Station name: %s\n", cfg_rec->station_name);
    printf("    Recorder id: %s\n", cfg_rec->rec_dev_id);
    printf("    Format revision: %d\n", cfg_rec->rev_year);
    printf("Channels info:\n");
    printf("    Total channels count: %d\n", cfg_rec->ch_count);
    printf("    Analog channels count: %d\n", cfg_rec->an_count);
    printf("    Digital channels count: %d\n", cfg_rec->dn_count);
    print_achannels_info(cfg_rec);
    print_dchannels_info(cfg_rec);
}

/* return codes:
 * 0 - Ok
 * 1 - file read error
*/
int main(int argc, char **argv)
{
    int res;
    FILE *fd;
    cmtrd_cfg_t cfg_rec;

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
