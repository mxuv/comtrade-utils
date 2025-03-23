#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comtrade-info_msg.h"
#include "strutils.h"
#include "format.h"

#define STR_BUFSIZE                         4096

#define EXIT_MEMERR()                       exit(5)

#define IS_CORRECT_INTPARAM_VAL(a, b, c)    is_correct_param_length(a, b, c)

enum getstring_status {
    gss_ok,
    gss_empty,
    gss_eof,
    gss_err,
    gss_overflow
};

enum analyze_cfg_state {
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

typedef struct {
    const char *str;
    int strlen;
    int nstr;
    int param_count;
} cfg_str_t;

typedef struct {
    enum param_type ptype;
    int num;
    int err;
    int len_min;
    int len_max;
    long int ival_min;
    long int ival_max;
    double dval_min;
    double dval_max;
} cfg_pvv_t;

const char ff_ascii[] = "ASCII";
const char ff_binary[] = "BINARY";
const char ff_binary32[] = "BINARY32";
const char ff_float32[] = "FLOAT32";

const char *ffv[] = {ff_ascii, ff_binary, ff_binary32, ff_float32};

const cfg_pvv_t sname = {
    ptstring,
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
    ptstring,
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
    ptint,
    PM_YEAR_POS,
    PM_ERR_REVYEAR,
    REVYEAR_LEN_MIN,
    REVYEAR_LEN_MAX,
    REV_YEAR_VAL_MIN,
    REV_YEAR_VAL_MAX,
    0,
    0
};
const cfg_pvv_t tt = {
    ptint,
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
    ptintc,
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
    ptintc,
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
    ptint,
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
    ptstring,
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
    ptstring,
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
    ptstring,
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
    ptstring, 
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
    ptfloat,
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
    ptfloat,
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
    ptfloat,
    PM_SKEW_POS,
    PM_ERR_SKEW,
    A_SKEW_LEN_MIN,
    A_SKEW_LEN_MAX,
    0,
    0,
    0,
    0
}; const cfg_pvv_t ach_min = {
    ptfloat,
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
    ptfloat,
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
    ptfloat,
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
    ptfloat,
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
    ptchar,
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
    ptint,
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
    ptstring,
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
    ptstring,
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
    ptstring,
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
    ptint,
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
    ptint,
    PM_Y_1999_POS,
    PM_ERR_Y,
    D_Y_LEN_MIN,
    D_Y_LEN_MAX,
    D_Y_VAL_MIN,
    D_Y_VAL_MAX,
    0,
    0
};
const cfg_pvv_t lf = {
    ptfloat,
    PM_LF_POS,
    PM_ERR_LF,
    LF_LEN_MIN,
    LF_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t nrates = {
    ptint,
    PM_NRATES_POS,
    PM_ERR_NRATES,
    NRATES_LEN_MIN,
    NRATES_LEN_MAX,
    NRATES_VAL_MIN,
    NRATES_VAL_MAX,
    0,
    0
};
const cfg_pvv_t samp = {
    ptfloat,
    PM_SAMP_POS,
    PM_ERR_SAMP,
    SAMP_LEN_MIN,
    SAMP_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t endsamp = {
    ptint,
    PM_ENDSAMP_POS,
    PM_ERR_ENDSAMP,
    ENDSAMP_LEN_MIN,
    ENDSAMP_LEN_MAX,
    ENDSAMP_VAL_MIN,
    ENDSAMP_VAL_MAX,
    0,
    0
};
const cfg_pvv_t startdate = {
    ptstring,
    PM_DATE_POS,
    PM_ERR_DATE,
    DATE_LEN_MIN,
    DATE_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t starttime = {
    ptstring,
    PM_TIME_POS,
    PM_ERR_TIME,
    TIME_LEN_MIN,
    TIME_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t day = {
    ptdate,
    PM_DAY_POS,
    PM_ERR_DAY,
    DAY_LEN_MIN,
    DAY_LEN_MAX,
    DAY_VAL_MIN,
    DAY_VAL_MAX,
    0,
    0
};
const cfg_pvv_t mon = {
    ptdate,
    PM_MON_POS,
    PM_ERR_MON,
    MON_LEN_MIN,
    MON_LEN_MAX,
    MON_VAL_MIN,
    MON_VAL_MAX,
    0,
    0
};
const cfg_pvv_t year = {
    ptdate,
    PM_YEAR_POS,
    PM_ERR_YEAR,
    YEAR_LEN_MIN,
    YEAR_LEN_MAX,
    YEAR_VAL_MIN,
    YEAR_VAL_MAX,
    0,
    0
};
const cfg_pvv_t hours = {
    pttime,
    PM_HOUR_POS,
    PM_ERR_HOUR,
    HOUR_LEN_MIN,
    HOUR_LEN_MAX,
    HOUR_VAL_MIN,
    HOUR_VAL_MAX,
    0,
    0
};
const cfg_pvv_t minuts = {
    pttime,
    PM_MINUT_POS,
    PM_ERR_MINUT,
    MIN_LEN_MIN,
    MIN_LEN_MAX,
    MIN_VAL_MIN,
    MIN_VAL_MAX,
    0,
    0
};
const cfg_pvv_t seconds = {
    ptstime,
    PM_SECONDS_POS,
    PM_ERR_SECONDS,
    SECONDS_LEN_MIN,
    SECONDS_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t seconds_p = {
    ptsecond,
    PM_SECONDS_P_POS,
    PM_ERR_SECONDS,
    SECONDS_P_LEN_MIN,
    SECONDS_P_LEN_MAX,
    SECONDS_P_VAL_MIN,
    SECONDS_P_VAL_MAX,
    0,
    0
};
const cfg_pvv_t seconds_s = {
    ptsecond,
    PM_SECONDS_S_POS,
    PM_ERR_SECONDS,
    SECONDS_S_LEN_MIN,
    SECONDS_S_LEN_MAX,
    SECONDS_S_VAL_MIN,
    SECONDS_S_VAL_MAX,
    0,
    0
};
const cfg_pvv_t filetype = {
    ptstring,
    PM_FILETYPE_POS,
    PM_ERR_FILETYPE,
    FILE_TYPE_LEN_MIN,
    FILE_TYPE_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t timemult = {
    ptfloat,
    PM_TIMEMULT_POS,
    PM_ERR_TIMEMULT,
    TIMEMULT_LEN_MIN,
    TIMEMULT_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t timecode = {
    ptstring,
    PM_TC_POS,
    PM_ERR_TC,
    TIME_CODE_LEN_MIN,
    TIME_CODE_LEN_MAX,
    0,
    0,
    0,
    0
};
const cfg_pvv_t localcode = {
    ptstring,
    PM_LC_POS,
    PM_ERR_LC,
    LOCAL_CODE_LEN_MIN,
    LOCAL_CODE_LEN_MAX,
    0,
    0,
    0,
    0
};

const cfg_pvv_t *pvv[] = { &sname, &recdevid, &revyear, &tt, &tt_a, &tt_d,
    &ach_num, &ach_chid, &ach_phase, &ach_ccbm, &ach_uu, &ach_a, &ach_b,
    &ach_skew, &ach_min, &ach_max, &ach_primary, &ach_secondary, &ach_ps,
    &dch_num, &dch_chid, &dch_phase, &dch_ccbm, &dch_y1991, &dch_y1999, &lf,
    &nrates, &samp, &endsamp, &startdate, &starttime, &day, &mon, &year,
    &hours, &minuts, &seconds, &seconds_p, &seconds_s, &filetype, &timemult,
    &timecode, &localcode};

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

int get_param_count(const char *str, char separator, int len)
{
    int count = 0;
    while (len) {
        if (*str == separator) 
            count++;
        str++;
        len--;
    }
    return count;
}

void get_param(char *dest, const char *str, int index, int len)
{
    stringcopy_c(dest, str+index, len);
}

int get_param_index(const char *str, int param, char separator)
{
    int param_curr = 0;
    const char *p;

    p = str;
    while (param_curr != param) {
        if (match_char(*str, separator))
            param_curr++;
        str++;
    }
    return str - p;
}

int get_param_length(const char *str, int stringlen, int param,
    int param_count, char separator)
{
    int index;

    index = get_param_index(str, param, separator);
    if ((param + 1) == param_count) {
        if (match_char(*(str+(stringlen - 1)), '\r'))
            return stringlen - 1 - index;
        else
            return stringlen - index;
    }
    else
        return get_param_index(str, param + 1, separator) - index - 1;
}

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

void add_error_code(int line, int strcode, long int paramcode,
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
        memset(p, 0, sizeof(cmtrd_an_t) * cfg_rec->an_count);
        cfg_rec->anv = (cmtrd_an_t*)p;
    }
    if (cfg_rec->dn_count) {
        p = malloc(sizeof(cmtrd_dn_t) * cfg_rec->dn_count);
        if (p == NULL)
            EXIT_MEMERR();
        memset(p, 0, sizeof(cmtrd_dn_t) * cfg_rec->dn_count);
        cfg_rec->dnv = (cmtrd_dn_t*)p;
    }
}

void create_samp_fields(cmtrd_cfg_t *cfg_rec)
{
    cmtrd_samp_t *p;

    p = malloc(sizeof(cmtrd_samp_t) * cfg_rec->real_nrates);
    if (p == NULL)
        EXIT_MEMERR();
    memset(p, 0, sizeof(cmtrd_samp_t) * cfg_rec->real_nrates);
    cfg_rec->samps = p;
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

void check_parameter_ival(cfg_pm_t *param, long int min, long int max)
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

int get_empty_field(void *p, int psize, int count, int fieldsize)
{
    int i;
    for (i = 0; i < count; i++) {
        if (psize == sizeof(int)) {
            if (*((int*)p + (i * fieldsize / sizeof(int))) == 0)
                return i;
        }
        if (psize == sizeof(long int)) {
            if (*((long int*)p + (i * fieldsize / sizeof(long int))) == 0)
                return i;
        }
    }
    return -1;
}

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

void save_fileformat(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    char paramstr[FILE_TYPE_LEN_MAX + 1];
    enum file_format ff;

    cfg_rec->ft = undef;
    if (pm->err & ERRCODE(LN_ERR_INCORRECT_PARAM_LEN))
        return;
    stringcopy_c(paramstr, cfg_str->str + pm->index, pm->len);
    upcase_string(paramstr);
    for (ff = 0; ff < undef; ff++) {
        if (stringmatch(paramstr, ffv[ff])) {
            cfg_rec->ft = ff;
            break;
        }
    }
    if (cfg_rec->ft == undef)
        pm->err |= ERRCODE(LN_ERR_INCORRECT_PARAM);
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
    case plf:
        cfg_rec->frequency = pm->val_float;
        break;
    case pnrates:
        cfg_rec->nrates = pm->val_int;
        break;
    case psamp:
        (cfg_rec->samps + pm->ch_index)->samp = pm->val_float;
        break;
    case pendsamp:
        (cfg_rec->samps + pm->ch_index)->end_samp = pm->val_int;
        break;
    case pfiletype:
        save_fileformat(pn, cfg_str, pm, cfg_rec);
        break;
    default:
        break;
    }
}

char set_separator(enum param_type ptype)
{
    switch (ptype) {
    case ptdate:
        return '/';
    case pttime:
    case ptstime:
        return ':';
    case ptsecond:
        return '.';
    default:
        return ',';
    }
}

void parsing_parameter(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    char s[PARAM_LEN_MAX+1];
    char c;

    c = set_separator(pvv[pn]->ptype);
    pm->err = 0;
    pm->index = get_param_index(cfg_str->str, pvv[pn]->num, c);
    pm->len = get_param_length(cfg_str->str, cfg_str->strlen, pvv[pn]->num,
            cfg_str->param_count, c);
    check_parameter_len(pm, pvv[pn]->len_min, pvv[pn]->len_max);
    switch (pvv[pn]->ptype) {
    case ptstring:
    case ptchar:
    case ptstime:
        break;
    case ptint:
    case ptdate:
    case pttime:
    case ptsecond:
        stringcopy_c(s, cfg_str->str + pm->index, pm->len);
        pm->val_int = atoi(s);
        check_parameter_ival(pm, pvv[pn]->ival_min, pvv[pn]->ival_max);
        break;
    case ptintc:
        stringcopy_c(s, cfg_str->str + pm->index, pm->len - 1);
        pm->val_int = atoi(s);
        check_parameter_ival(pm, pvv[pn]->ival_min, pvv[pn]->ival_max);
        break;
    case ptfloat:
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
    pm.ch_index = get_empty_field(&cfg_rec->anv->num, sizeof(int),
            cfg_rec->an_count, sizeof(cmtrd_an_t));
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

int analyze_cfg_dchannel(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;

    error = check_param_count(cfg_str->param_count, CP_DN_1991, CP_DN_1999);
    pm.ch_index = get_empty_field(&cfg_rec->dnv->num, sizeof(int),
            cfg_rec->dn_count, sizeof(cmtrd_dn_t));
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
    } else {
    /* Channel default state */
        parsing_parameter(pdy_1991, cfg_str, &pm, cfg_rec);
    }

    return pm.ch_index;
}

void analyze_cfg_line_frequency(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_LF, CP_LF);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 

    /* Line frequency */
    parsing_parameter(plf, cfg_str, &pm, cfg_rec);
}

int analyze_cfg_nrates(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_NRATES, CP_NRATES);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 

    /* Number of sample rates */
    error = 0;
    parsing_parameter(pnrates, cfg_str, &pm, cfg_rec);
    if (pm.err && ERRCODE(LN_ERR_INCORRECT_PARAM))
        error++;
    if (!cfg_rec->nrates)
        cfg_rec->real_nrates = 1;
    else
        cfg_rec->real_nrates = cfg_rec->nrates;

    return error;
}

int analyze_cfg_samp(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;

    error = check_param_count(cfg_str->param_count, CP_SAMP, CP_SAMP);
    pm.ch_index = get_empty_field(&cfg_rec->samps->end_samp, sizeof(long int),
            cfg_rec->real_nrates, sizeof(cmtrd_samp_t));
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return pm.ch_index;
    if (pm.ch_index == -1)
        return pm.ch_index;

    parsing_parameter(psamp, cfg_str, &pm, cfg_rec);
    parsing_parameter(pendsamp, cfg_str, &pm, cfg_rec);
    return pm.ch_index;
}

void init_substring(cfg_str_t *str, cfg_str_t *substr, cfg_pm_t *pm,
        int offset, char separator)
{
    substr->str = str->str + offset;
    substr->nstr = str->nstr;
    substr->strlen = pm->len;
    substr->param_count = get_param_count(substr->str, separator,
            substr->strlen) + 1;
}

void parsing_date(cfg_str_t *cfg_str, cfg_pm_t *pm, cmtrd_timestamp_t *dt,
        cmtrd_cfg_t *cfg_rec)
{
    cfg_str_t substr;
    cfg_pm_t subpm;

    init_substring(cfg_str, &substr, pm, 0, '/');
    if (substr.param_count != CP_DATE) {
        add_error_code(substr.nstr, ERRCODE(LN_ERR_INCORRECT_PARAM),
                ERRCODE(PM_ERR_DATE), cfg_rec);
        return;
    }
    parsing_parameter(pday, &substr, &subpm, cfg_rec);
    dt->day = subpm.val_int;
    parsing_parameter(pmon, &substr, &subpm, cfg_rec);
    dt->mon = subpm.val_int;
    parsing_parameter(pyear, &substr, &subpm, cfg_rec);
    dt->year = subpm.val_int;
}

void parsing_time_seconds(cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_timestamp_t *dt, cmtrd_cfg_t *cfg_rec)
{
    cfg_str_t substr;
    cfg_pm_t subpm;

    init_substring(cfg_str, &substr, pm, pm->index, '.');
    if (substr.param_count != CP_SECONDS) {
        add_error_code(substr.nstr, ERRCODE(LN_ERR_INCORRECT_PARAM),
                ERRCODE(PM_ERR_TIME), cfg_rec);
        return;
    }
    parsing_parameter(pseconds_p, &substr, &subpm, cfg_rec);
    dt->sec = subpm.val_int;
    parsing_parameter(pseconds_s, &substr, &subpm, cfg_rec);
    dt->subsec = subpm.val_int;
}

void parsing_time(cfg_str_t *cfg_str, cfg_pm_t *pm, cmtrd_timestamp_t *dt,
        cmtrd_cfg_t *cfg_rec)
{
    cfg_str_t substr;
    cfg_pm_t subpm;

    init_substring(cfg_str, &substr, pm, pm->index, ':');
    if (substr.param_count != CP_TIME) {
        add_error_code(substr.nstr, ERRCODE(LN_ERR_INCORRECT_PARAM),
                ERRCODE(PM_ERR_TIME), cfg_rec);
        return;
    }
    parsing_parameter(phours, &substr, &subpm, cfg_rec);
    dt->hour = subpm.val_int;
    parsing_parameter(pminuts, &substr, &subpm, cfg_rec);
    dt->min = subpm.val_int;
    parsing_parameter(pseconds, &substr, &subpm, cfg_rec);
    if (subpm.len)
        parsing_time_seconds(&substr, &subpm, dt, cfg_rec);
}

void analyze_cfg_datetime(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec,
        enum analyze_cfg_state state)
{
    int error;
    cfg_pm_t pm;
    cmtrd_timestamp_t dt;

    error = check_param_count(cfg_str->param_count, CP_DATE_TIME, CP_DATE_TIME);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return;

    parsing_parameter(pstdate, cfg_str, &pm, cfg_rec);
    if (pm.len)
        parsing_date(cfg_str, &pm, &dt, cfg_rec);
    parsing_parameter(psttime, cfg_str, &pm, cfg_rec);
    if (pm.len)
        parsing_time(cfg_str, &pm, &dt, cfg_rec);
    if (state == analyze_sdatetime)
        memcpy(&cfg_rec->start_datetime, &dt, sizeof(dt));
    else if (state == analyze_trigdatetime)
        memcpy(&cfg_rec->trig_datetime, &dt, sizeof(dt));
}

void analyze_cfg_filetype(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;

    error = check_param_count(cfg_str->param_count, CP_FILETYPE, CP_FILETYPE);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 

    /* Data file type */
    parsing_parameter(pfiletype, cfg_str, &pm, cfg_rec);
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

        cfg_str.strlen--;
        cfg_str.str = buffer;
        cfg_str.param_count = get_param_count(buffer, ',', cfg_str.strlen) + 1;
        switch (next_state) {
        case analyze_header:
            analyze_cfg_header(&cfg_str, cfg_rec);
            next_state++;
            break;
        case analyze_tt:
            if (analyze_cfg_chinfo(&cfg_str, cfg_rec))
                return 1;
            create_channels_fields(cfg_rec);
            if (cfg_rec->an_count)
                next_state = analyze_ach;
            else
                next_state = analyze_dch;
            break;
        case analyze_ach:
            result = analyze_cfg_achannel(&cfg_str, cfg_rec);
            if (result == -1 || result == cfg_rec->an_count - 1) {
                if (cfg_rec->dn_count)
                    next_state = analyze_dch;
                else
                    next_state = analyze_lf;
            }
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
        case analyze_lf:
            analyze_cfg_line_frequency(&cfg_str, cfg_rec);
            next_state++;
            break;
        case analyze_nrates:
            if (analyze_cfg_nrates(&cfg_str, cfg_rec))
                return 1;
            create_samp_fields(cfg_rec);
            next_state++;
            break;
        case analyze_samp:
            result = analyze_cfg_samp(&cfg_str, cfg_rec);
            if (result == -1 || result == cfg_rec->real_nrates - 1)
                next_state++;
            break;
        case analyze_sdatetime:
        case analyze_trigdatetime:
            analyze_cfg_datetime(&cfg_str, cfg_rec, next_state);
            next_state++;
            break;
        case analyze_filetype:
            analyze_cfg_filetype(&cfg_str, cfg_rec);
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
            for (msg_index = 0; msg_index < PM_ERR_COUNT; msg_index++) {
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
        printf("    %s: %d\n", parammsg[6], (cfg_rec->anv + i)->num);
        printf("    %s: %s\n", parammsg[7], (cfg_rec->anv + i)->ch_id);
        printf("    %s: %s\n", parammsg[8], (cfg_rec->anv + i)->phase);
        printf("    %s: %s\n", parammsg[9], (cfg_rec->anv + i)->ccbm);
        printf("    %s: %s\n", parammsg[10], (cfg_rec->anv + i)->uu);
        printf("    %s: %lf\n", parammsg[11], (cfg_rec->anv + i)->a);
        printf("    %s: %lf\n", parammsg[12], (cfg_rec->anv + i)->b);
        printf("    %s: %lf\n", parammsg[13], (cfg_rec->anv + i)->skew);
        printf("    %s: %lf\n", parammsg[14], (cfg_rec->anv + i)->min);
        printf("    %s: %lf\n", parammsg[15], (cfg_rec->anv + i)->max);
        printf("    %s: %lf\n", parammsg[16], (cfg_rec->anv + i)->primary);
        printf("    %s: %lf\n", parammsg[17], (cfg_rec->anv + i)->secondary);
        printf("    %s: %c\n", parammsg[18], (cfg_rec->anv + i)->ps);

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
    int i;

    printf("General info:\n");
    printf("    %s: %s\n", parammsg[0], cfg_rec->station_name);
    printf("    %s: %s\n", parammsg[1], cfg_rec->rec_dev_id);
    printf("    %s: %d\n", parammsg[2], cfg_rec->rev_year);
    printf("    %s: %lf\n", parammsg[21], cfg_rec->frequency);
    printf("    %s: %d\n", parammsg[22], cfg_rec->nrates);
    printf("    Start timestamp: %02d/%02d/%d %02d:%02d:%02d.%d\n", 
            cfg_rec->start_datetime.day, cfg_rec->start_datetime.mon,
            cfg_rec->start_datetime.year, cfg_rec->start_datetime.hour,
            cfg_rec->start_datetime.min, cfg_rec->start_datetime.sec,
            cfg_rec->start_datetime.subsec);
    printf("    Trigger timestamp: %02d/%02d/%d %02d:%02d:%02d.%d\n", 
            cfg_rec->trig_datetime.day, cfg_rec->trig_datetime.mon,
            cfg_rec->trig_datetime.year, cfg_rec->trig_datetime.hour,
            cfg_rec->trig_datetime.min, cfg_rec->trig_datetime.sec,
            cfg_rec->trig_datetime.subsec);
    for (i = 0; i < cfg_rec->real_nrates; i++) {
        printf("    %s: %lf\n", parammsg[23], (cfg_rec->samps + i)->samp);
        printf("    %s: %ld\n", parammsg[24], (cfg_rec->samps + i)->end_samp);
    }
    if (cfg_rec->ft == undef)
        printf("    %s: Undefined\n", parammsg[31]);
    else
        printf("    %s: %s\n", parammsg[31], ffv[cfg_rec->ft]);

    printf("Channels info:\n");
    printf("    %s: %d\n", parammsg[3], cfg_rec->ch_count);
    printf("    %s: %d\n", parammsg[4], cfg_rec->an_count);
    printf("    %s: %d\n", parammsg[5], cfg_rec->dn_count);
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
