#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <strutils/strutils.h>

#include "cfg.h"

#define EXIT_MEMERR()                       exit(5)

#define IS_CORRECT_INTPARAM_VAL(a, b, c)    is_correct_param_length(a, b, c)

#define STR_BUFSIZE                         4096

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
    analyze_tmqcode,
    analyze_end,
    analyze_afterend
};

enum getstring_status {
    gss_ok,
    gss_empty,
    gss_eof,
    gss_err,
    gss_overflow
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

const char default_strfield[] = "N/A";

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
const cfg_pvv_t tmqcode = {
    ptcharhex,
    PM_TMQ_POS,
    PM_ERR_TMQ,
    TMQ_CODE_LEN_MIN,
    TMQ_CODE_LEN_MAX,
    TMQ_VAL_MIN,
    TMQ_VAL_MAX,
    0,
    0
};
const cfg_pvv_t leapsec = {
    ptint,
    PM_LEAPSEC_POS,
    PM_ERR_LEAPSEC,
    LEAPSEC_LEN_MIN,
    LEAPSEC_LEN_MAX,
    LEAPSEC_VAL_MIN,
    LEAPSEC_VAL_MAX,
    0,
    0
};

const cfg_pvv_t *pvv[] = { &sname, &recdevid, &revyear, &tt, &tt_a, &tt_d,
    &ach_num, &ach_chid, &ach_phase, &ach_ccbm, &ach_uu, &ach_a, &ach_b,
    &ach_skew, &ach_min, &ach_max, &ach_primary, &ach_secondary, &ach_ps,
    &dch_num, &dch_chid, &dch_phase, &dch_ccbm, &dch_y1991, &dch_y1999, &lf,
    &nrates, &samp, &endsamp, &startdate, &starttime, &day, &mon, &year,
    &hours, &minuts, &seconds, &seconds_p, &seconds_s, &filetype, &timemult,
    &timecode, &localcode, &tmqcode, &leapsec};

static int match_char(char ch, char patt)
{
    if (ch == patt)
        return 1;
    else
        return 0;
}

static int getstring(FILE *fd, char *buffer, int bufsize,
                enum getstring_status *status)
{
    int len = 0;

    *status = gss_ok;
    if (fgets(buffer, bufsize, fd) != NULL) {
        len = strlen(buffer);
        if (buffer[len-1] != '\n') {
            if (len == bufsize - 1) {
                *status  = gss_overflow;
		return len;
	    }
        }
    } else {
	if (feof(fd))
	    *status = gss_eof;
	else
	    *status = gss_err;
    }

    return len;
}

static int is_line_ending_ok(const char *str, int len)
{
    if (str[len-2] == '\r')
        return 1;
    else
        return 0;
}

static int get_param_count(const char *str, char separator, int len)
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

static int get_param_index(const char *str, int param, char separator)
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

static int get_param_length(const char *str, int stringlen, int param,
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

static void add_error_field(cmtrd_cfg_t *cfg_rec)
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

static void add_error_code(int line, int strcode, long int paramcode,
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

static int is_correct_param_length(int len, int min, int max)
{
    if (len >= min && len <= max)
        return 1;
    else
        return 0;
}

static char* add_str_item(const char *src, int length)
{
    char *str;

    str = malloc(sizeof(char) * length + 1);
    if (str == NULL)
        EXIT_MEMERR();

    stringcopy_c(str, src, length);
    return str;
}

static void set_default_ach_strfields(cmtrd_cfg_t *cfg_rec)
{
    int i;

    for (i = 0; i < cfg_rec->an_count; i++) {
        (cfg_rec->anv + i)->ch_id = (char*)default_strfield;
        (cfg_rec->anv + i)->phase = (char*)default_strfield;
        (cfg_rec->anv + i)->ccbm = (char*)default_strfield;
        (cfg_rec->anv + i)->uu = (char*)default_strfield;
    }
}

static void set_default_dch_strfields(cmtrd_cfg_t *cfg_rec)
{
    int i;

    for (i = 0; i < cfg_rec->dn_count; i++) {
        (cfg_rec->dnv + i)->ch_id = (char*)default_strfield;
        (cfg_rec->dnv + i)->phase = (char*)default_strfield;
        (cfg_rec->dnv + i)->ccbm = (char*)default_strfield;
    }
}

static void create_channels_fields(cmtrd_cfg_t *cfg_rec)
{
    void *p;

    if (cfg_rec->an_count) {
        p = malloc(sizeof(cmtrd_an_t) * cfg_rec->an_count);
        if (p == NULL)
            EXIT_MEMERR();
        memset(p, 0, sizeof(cmtrd_an_t) * cfg_rec->an_count);
        cfg_rec->anv = (cmtrd_an_t*)p;
        set_default_ach_strfields(cfg_rec);
    }
    if (cfg_rec->dn_count) {
        p = malloc(sizeof(cmtrd_dn_t) * cfg_rec->dn_count);
        if (p == NULL)
            EXIT_MEMERR();
        memset(p, 0, sizeof(cmtrd_dn_t) * cfg_rec->dn_count);
        cfg_rec->dnv = (cmtrd_dn_t*)p;
        set_default_dch_strfields(cfg_rec);
    }
}

static void create_samp_fields(cmtrd_cfg_t *cfg_rec)
{
    cmtrd_samp_t *p;

    p = malloc(sizeof(cmtrd_samp_t) * cfg_rec->real_nrates);
    if (p == NULL)
        EXIT_MEMERR();
    memset(p, 0, sizeof(cmtrd_samp_t) * cfg_rec->real_nrates);
    cfg_rec->samps = p;
}

static int check_param_count(int count, int min, int max)
{
    if (count < min)
        return ERRCODE(LN_ERR_TOO_FEW_PARAM);
    if (count > max)
        return ERRCODE(LN_ERR_TOO_MANY_PARAM);
    return 0;
}

static void check_parameter_len(cfg_pm_t *param, int min, int max)
{
    if (!is_correct_param_length(param->len, min, max))
        param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM_LEN);
}

static void check_parameter_ival(cfg_pm_t *param, long int min, long int max)
{
    if (!IS_CORRECT_INTPARAM_VAL(param->val_int, min, max))
        param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM);
}

static int is_correct_dparam_val(double val, double min, double max)
{
    if (min == 0 && max == 0)
        return 1;

    if (val > min && val < max)
        return 1;
    else
        return 0;
}

static void check_parameter_dval(cfg_pm_t *param, double min, double max)
{
    if (!is_correct_dparam_val(param->val_float, min, max))
    param->err |= ERRCODE(LN_ERR_INCORRECT_PARAM);
}

static char check_ps_value(cfg_str_t *cfg_str, cfg_pm_t *pm)
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

static int get_empty_field(void *p, int psize, int count, int fieldsize)
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

static int is_match_ach_rev(int param_count, int rev_year)
{
    if (rev_year >= rev1991 && rev_year < rev1999 && param_count == CP_AN_1991)
        return 1;

    if (rev_year >= rev1999 && param_count == CP_AN_1999)
        return 1;

    return 0;
}

static int is_match_dch_rev(int param_count, int rev_year)
{
    if (rev_year >= rev1991 && rev_year < rev1999 && param_count == CP_DN_1991)
        return 1;

    if (rev_year >= rev1999 && param_count == CP_DN_1999)
        return 1;

    return 0;
}

static void save_header_items(enum cfg_pnum pn, cfg_str_t *cfg_str,
        cfg_pm_t *pm, cmtrd_cfg_t *cfg_rec)
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

static void save_chinfo_items(enum cfg_pnum pn, cfg_pm_t *pm,
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

 static void save_achannel_items(enum cfg_pnum pn, cfg_str_t *cfg_str,
         cfg_pm_t *pm, cmtrd_cfg_t *cfg_rec)
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

 static void save_dchannel_items(enum cfg_pnum pn, cfg_str_t *cfg_str,
         cfg_pm_t *pm, cmtrd_cfg_t *cfg_rec)
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

static void save_fileformat(cfg_str_t *cfg_str, cfg_pm_t *pm,
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

static void save_value2rec(enum cfg_pnum pn, cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_cfg_t *cfg_rec)
{
    if (pn >= psname && pn <= prev_year) {
        save_header_items(pn, cfg_str, pm, cfg_rec);
        return;
    }

    if (pn >= ptt && pn <= ptt_d) {
        save_chinfo_items(pn, pm, cfg_rec);
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
        save_fileformat(cfg_str, pm, cfg_rec);
        break;
    case ptimemult:
        cfg_rec->timemult = pm->val_float;
        break;
    case ptimecode:
        cfg_rec->time_code = add_str_item(cfg_str->str + pm->index, pm->len);
        break;
    case plocalcode:
        cfg_rec->local_code = add_str_item(cfg_str->str + pm->index, pm->len);
        break;
    case ptmqcode:
        cfg_rec->tmq_code = pm->val_int;
        break;
    case pleapsec:
        cfg_rec->leapsec = pm->val_int;
        break;
    default:
        break;
    }
}

static char set_separator(enum param_type ptype)
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

static void parsing_parameter(enum cfg_pnum pn, cfg_str_t *cfg_str,
        cfg_pm_t *pm, cmtrd_cfg_t *cfg_rec)
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
    case ptcharhex:
        c = cfg_str->str[pm->index];
        pm->val_int = char2int(c);
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

static int analyze_cfg_header(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
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
static int analyze_cfg_chinfo(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
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
static int analyze_cfg_achannel(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
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

static int analyze_cfg_dchannel(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
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

static void analyze_cfg_line_frequency(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_LF, CP_LF);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 

    /* Line frequency */
    parsing_parameter(plf, cfg_str, &pm, cfg_rec);
}

static int analyze_cfg_nrates(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
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

static int analyze_cfg_samp(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
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

static void init_substring(cfg_str_t *str, cfg_str_t *substr, cfg_pm_t *pm,
        int offset, char separator)
{
    substr->str = str->str + offset;
    substr->nstr = str->nstr;
    substr->strlen = pm->len;
    substr->param_count = get_param_count(substr->str, separator,
            substr->strlen) + 1;
}

static void parsing_date(cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_timestamp_t *dt, cmtrd_cfg_t *cfg_rec)
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

static void parsing_time_seconds(cfg_str_t *cfg_str, cfg_pm_t *pm,
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

static void parsing_time(cfg_str_t *cfg_str, cfg_pm_t *pm,
        cmtrd_timestamp_t *dt, cmtrd_cfg_t *cfg_rec)
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

static void analyze_cfg_datetime(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec,
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

static void analyze_cfg_filetype(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;

    error = check_param_count(cfg_str->param_count, CP_FILETYPE, CP_FILETYPE);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 

    /* Data file type */
    parsing_parameter(pfiletype, cfg_str, &pm, cfg_rec);
}

static void analyze_cfg_timemult(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_TIMEMULT, CP_TIMEMULT);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 

    /* Line frequency */
    parsing_parameter(ptimemult, cfg_str, &pm, cfg_rec);
}

static void analyze_cfg_timecode(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_TIME_CODE, CP_TIME_CODE);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return ;

    /* Time code */
    parsing_parameter(ptimecode, cfg_str, &pm, cfg_rec);

    /* Local code  */
    parsing_parameter(plocalcode, cfg_str, &pm, cfg_rec);
}

void analyze_cfg_tmq(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec)
{
    int error;
    cfg_pm_t pm;
    
    error = check_param_count(cfg_str->param_count, CP_TMQ_CODE, CP_TMQ_CODE);
    if (error)
        add_error_code(cfg_str->nstr, error, ERRNULL, cfg_rec); 
    if (error & ERRCODE(LN_ERR_TOO_FEW_PARAM))
        return ;

    /* TMQ code */
    parsing_parameter(ptmqcode, cfg_str, &pm, cfg_rec);

    /* Leapsec  */
    parsing_parameter(pleapsec, cfg_str, &pm, cfg_rec);
}

void check_lnend_err(char *buffer, cfg_str_t *str, cmtrd_cfg_t *rec)
{
    if (!is_line_ending_ok(buffer, str->strlen))
        add_error_code(str->nstr, ERRCODE(LN_ERR_NOCR), ERRNULL, rec);
}

int state_analyze_ach(cfg_str_t *str, cmtrd_cfg_t *rec)
{
    int result;

    result = analyze_cfg_achannel(str, rec);
    if (result == -1 || result == rec->an_count - 1) {
        if (rec->dn_count)
            result = analyze_dch;
        else
            result  = analyze_lf;
    } else {
        result = analyze_ach;
    }

    if (!is_match_ach_rev(str->param_count, rec->rev_year))
        add_error_code(str->nstr, ERRCODE(LN_ERR_MATCH_REV_YEAR), ERRNULL, rec);

    return result;
}

int state_analyze_dch(cfg_str_t *str, cmtrd_cfg_t *rec)
{
    int result;

    result = analyze_cfg_dchannel(str, rec);
    if (result == -1 || result == rec->dn_count - 1)
        result = analyze_lf;
    else
        result = analyze_dch;
    if (!is_match_dch_rev(str->param_count, rec->rev_year))
        add_error_code(str->nstr, ERRCODE(LN_ERR_MATCH_REV_YEAR),
            ERRNULL, rec);
    return result;
}

int parsing_string(cfg_str_t *cfg_str, cmtrd_cfg_t *cfg_rec,
        enum analyze_cfg_state *state)
{
    int result;
    switch (*state) {
    case analyze_header:
        analyze_cfg_header(cfg_str, cfg_rec);
        *state = analyze_tt;
        break;
    case analyze_tt:
        if (analyze_cfg_chinfo(cfg_str, cfg_rec))
            return 1;
        create_channels_fields(cfg_rec);
        if (cfg_rec->an_count)
            *state = analyze_ach;
        else
            *state = analyze_dch;
        break;
    case analyze_ach:
        *state = state_analyze_ach(cfg_str, cfg_rec);
        break;
    case analyze_dch:
        *state = state_analyze_dch(cfg_str, cfg_rec);
        break;
    case analyze_lf:
        analyze_cfg_line_frequency(cfg_str, cfg_rec);
        *state = analyze_nrates;
        break;
    case analyze_nrates:
        if (analyze_cfg_nrates(cfg_str, cfg_rec))
            return 1;
        create_samp_fields(cfg_rec);
        *state = analyze_samp;
        break;
    case analyze_samp:
        result = analyze_cfg_samp(cfg_str, cfg_rec);
        if (result == -1 || result == cfg_rec->real_nrates - 1)
            *state = analyze_sdatetime;
        break;
    case analyze_sdatetime:
    case analyze_trigdatetime:
        analyze_cfg_datetime(cfg_str, cfg_rec, *state);
        (*state)++;
        break;
    case analyze_filetype:
        analyze_cfg_filetype(cfg_str, cfg_rec);
        if (cfg_rec->rev_year >= rev1999)
            *state = analyze_timemult;
        else
            *state = analyze_end;
        break;
    case analyze_timemult:
        analyze_cfg_timemult(cfg_str, cfg_rec);
        if (cfg_rec->rev_year == rev2013)
            *state = analyze_timecode;
        else
            *state = analyze_end;
        break;
    case analyze_timecode:
        analyze_cfg_timecode(cfg_str, cfg_rec);
        *state = analyze_tmqcode;
        break;
    case analyze_tmqcode:
        analyze_cfg_tmq(cfg_str, cfg_rec);
        *state = analyze_end;
        break;
    case analyze_end:
        cfg_rec->last_correctline = cfg_str->nstr;
        *state = analyze_afterend;
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

/* Return values:
 * 0-Ok
 * 1-Incorrect data. Analysis aborted
 * 2-Unexcepted end of file
 * 3-Read error
 * 4-Buffer overflow
 * 5-Extra lines in file
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
            break;
        check_lnend_err(buffer, &cfg_str, cfg_rec);
        cfg_str.strlen -= 1;
        cfg_str.str = buffer;
        cfg_str.param_count = get_param_count(buffer, ',', cfg_str.strlen) + 1;
        result = parsing_string(&cfg_str, cfg_rec, &next_state);
        cfg_str.nstr++;
        if (result)
            break;
    }

    cfg_rec->lastline = cfg_str.nstr;
    if (status == gss_err)
        return 3;
    if (status == gss_overflow)
        return 4;
    if (result)
        return result;
    if (next_state == analyze_afterend)
        return 5;
    if (next_state != analyze_end)
        return 2;

    return 0;
}

void cfg_record_init(cmtrd_cfg_t *cfg_rec)
{
    memset(cfg_rec, 0, sizeof(cmtrd_cfg_t));
    cfg_rec->rev_year = 1991;
    cfg_rec->station_name = (char*)default_strfield;
    cfg_rec->rec_dev_id = (char*)default_strfield;
    cfg_rec->time_code = (char*)default_strfield;
    cfg_rec->local_code = (char*)default_strfield;
}
