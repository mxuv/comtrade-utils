#ifndef _FORMAT_H_
#define _FORMAT_H_

/* Parameters count in line */
#define CP_HEADER_MAX               3                   
#define CP_HEADER_MIN               2                   

#define CP_TT                       3

#define CP_AN_1991                  10
#define CP_AN_1999                  13

#define CP_DN_1991                  3
#define CP_DN_1999                  5

#define CP_LF                       1

#define CP_NRATES                   1

#define CP_SAMP                     2

#define CP_DATE_TIME                2
#define CP_DATE                     3
#define CP_TIME                     3
#define CP_SECONDS                  2

#define CP_TRIGG_DATE_TIME          2

#define CP_FILETYPE                 1

#define CP_TIMEMULT                 1

#define CP_TIME_CODE                2
#define CP_TMQ_CODE                 2

#define PARAM_COUNT_MAX             37

/* Parameters length */
#define SNAME_LEN_MIN               0
#define SNAME_LEN_MAX               64

#define RECDEV_LEN_MIN              0
#define RECDEV_LEN_MAX              64
#define REVYEAR_LEN_MIN             4
#define REVYEAR_LEN_MAX             4

#define TT_LEN_MIN                  1
#define TT_LEN_MAX                  6
#define AN_LEN_MIN                  2
#define AN_LEN_MAX                  7
#define DN_LEN_MIN                  2
#define DN_LEN_MAX                  7

#define A_N_LEN_MIN                 1
#define A_N_LEN_MAX                 6
#define A_CHID_LEN_MIN              1
#define A_CHID_LEN_MAX              128
#define A_PH_LEN_MIN                0
#define A_PH_LEN_MAX                2
#define A_CCBM_LEN_MIN              0
#define A_CCBM_LEN_MAX              64
#define A_UU_LEN_MIN                1
#define A_UU_LEN_MAX                32
#define A_A_LEN_MIN                 1
#define A_A_LEN_MAX                 32
#define A_B_LEN_MIN                 1
#define A_B_LEN_MAX                 32
#define A_SKEW_LEN_MIN              1
#define A_SKEW_LEN_MAX              32
#define A_MINSC_LEN_MIN             1
#define A_MINSC_LEN_MAX             13
#define A_MAXSC_LEN_MIN             1
#define A_MAXSC_LEN_MAX             13
#define A_PRIMARY_LEN_MIN           1
#define A_PRIMARY_LEN_MAX           32
#define A_SECONDARY_LEN_MIN         1
#define A_SECONDARY_LEN_MAX         32
#define A_PS_LEN_MIN                1
#define A_PS_LEN_MAX                1

#define D_N_LEN_MIN                 1
#define D_N_LEN_MAX                 6
#define D_CHID_LEN_MIN              1
#define D_CHID_LEN_MAX              128
#define D_PH_LEN_MIN                0
#define D_PH_LEN_MAX                2
#define D_CCBM_LEN_MIN              0
#define D_CCBM_LEN_MAX              64
#define D_Y_LEN_MIN                 1
#define D_Y_LEN_MAX                 1

#define LF_LEN_MIN                  0
#define LF_LEN_MAX                  32

#define NRATES_LEN_MIN              1
#define NRATES_LEN_MAX              3
#define SAMP_LEN_MIN                1
#define SAMP_LEN_MAX                32
#define ENDSAMP_LEN_MIN             1
#define ENDSAMP_LEN_MAX             10

#define DAY_LEN_MIN                 1
#define DAY_LEN_MAX                 2
#define MON_LEN_MIN                 1
#define MON_LEN_MAX                 2
#define YEAR_LEN_MIN                4
#define YEAR_LEN_MAX                4
#define HOUR_LEN_MIN                2
#define HOUR_LEN_MAX                2
#define MIN_LEN_MIN                 2
#define MIN_LEN_MAX                 2
#define SECONDS_LEN_MIN             9
#define SECONDS_LEN_MAX             12
#define SECONDS_P_LEN_MIN           2
#define SECONDS_P_LEN_MAX           2
#define SECONDS_S_LEN_MIN           6
#define SECONDS_S_LEN_MAX           9

#define DATE_LEN_MIN                DAY_LEN_MIN + MON_LEN_MIN + YEAR_LEN_MIN + 2
#define DATE_LEN_MAX                DAY_LEN_MAX + MON_LEN_MAX + YEAR_LEN_MAX + 2
#define TIME_LEN_MIN                HOUR_LEN_MIN + MIN_LEN_MIN + \
                                    SECONDS_LEN_MIN + 2
#define TIME_LEN_MAX                HOUR_LEN_MAX + MIN_LEN_MAX + \
                                    SECONDS_LEN_MAX + 2

#define FILE_TYPE_LEN_MIN           5
#define FILE_TYPE_LEN_MAX           8

#define TIMEMULT_LEN_MIN            1
#define TIMEMULT_LEN_MAX            32

#define TIME_CODE_LEN_MIN           1
#define TIME_CODE_LEN_MAX           6
#define LOCAL_CODE_LEN_MIN          1
#define LOCAL_CODE_LEN_MAX          6

#define TMQ_CODE_LEN_MIN            1
#define TMQ_CODE_LEN_MAX            1
#define LEAPSEC_LEN_MIN             1 
#define LEAPSEC_LEN_MAX             1 

#define PARAM_LEN_MAX               128

/* Minimum and maximum values of parameters */
#define REV_YEAR_VAL_MIN            1991
#define REV_YEAR_VAL_MAX            2013

#define TT_VAL_MIN                  1
#define TT_VAL_MAX                  999999
#define AN_VAL_MIN                  0
#define AN_VAL_MAX                  999999
#define DN_VAL_MIN                  0
#define DN_VAL_MAX                  999999

#define A_N_VAL_MIN                 1
#define A_N_VAL_MAX                 999999
#define A_MINSC_VAL_MIN             -3.4028235E38
#define A_MINSC_VAL_MAX             3.4028235E38
#define A_MAXSC_VAL_MIN             -3.4028235E38
#define A_MAXSC_VAL_MAX             3.4028235E38

#define D_N_VAL_MIN                 1
#define D_N_VAL_MAX                 999999
#define D_Y_VAL_MIN                 0
#define D_Y_VAL_MAX                 1

#define NRATES_VAL_MIN              0
#define NRATES_VAL_MAX              999
#define ENDSAMP_VAL_MIN             1
#define ENDSAMP_VAL_MAX             9999999999

#define DAY_VAL_MIN                 1
#define DAY_VAL_MAX                 31
#define MON_VAL_MIN                 1
#define MON_VAL_MAX                 12
#define YEAR_VAL_MIN                1900
#define YEAR_VAL_MAX                9999
#define HOUR_VAL_MIN                0
#define HOUR_VAL_MAX                23
#define MIN_VAL_MIN                 0
#define MIN_VAL_MAX                 59
#define SECONDS_P_VAL_MIN           0
#define SECONDS_P_VAL_MAX           59
#define SECONDS_S_VAL_MIN           0
#define SECONDS_S_VAL_MAX           999999999

#define TMQ_VAL_MIN                 0
#define TMQ_VAL_MAX                 15

#define LEAPSEC_VAL_MIN             0 
#define LEAPSEC_VAL_MAX             3 

/* Line error codes */
#define LN_ERR_NOCR                 0
#define LN_ERR_EXTRA_SPACES         1
#define LN_ERR_TOO_MANY_PARAM       2
#define LN_ERR_TOO_FEW_PARAM        3
#define LN_ERR_INCORRECT_PARAM      4
#define LN_ERR_INCORRECT_PARAM_LEN  5
#define LN_ERR_INCORRECT_PARAM_SUM  6
#define LN_ERR_MATCH_REV_YEAR       7
#define LN_ERR_COUNT                8

/* Parameters error codes */
#define PM_ERR_SNAME                0
#define PM_ERR_REC_ID               1
#define PM_ERR_REVYEAR              2

#define PM_ERR_TT                   3
#define PM_ERR_TT_A                 4
#define PM_ERR_TT_D                 5

#define PM_ERR_AN                   6
#define PM_ERR_CHID                 7
#define PM_ERR_PH                   8
#define PM_ERR_CCBM                 9
#define PM_ERR_UU                   10
#define PM_ERR_A                    11
#define PM_ERR_B                    12
#define PM_ERR_SKEW                 13
#define PM_ERR_MIN                  14
#define PM_ERR_MAX                  15
#define PM_ERR_PRIM                 16
#define PM_ERR_SEC                  17
#define PM_ERR_PS                   18

#define PM_ERR_DN                   19
#define PM_ERR_Y                    20

#define PM_ERR_LF                   21
#define PM_ERR_NRATES               22
#define PM_ERR_SAMP                 23
#define PM_ERR_ENDSAMP              24

#define PM_ERR_DAY                  25
#define PM_ERR_MON                  26
#define PM_ERR_YEAR                 27
#define PM_ERR_HOUR                 28
#define PM_ERR_MINUT                29
#define PM_ERR_SECONDS              30
#define PM_ERR_FILETYPE             31
#define PM_ERR_TIMEMULT             32
#define PM_ERR_TC                   33
#define PM_ERR_LC                   34
#define PM_ERR_TMQ                  35
#define PM_ERR_LEAPSEC              36
#define PM_ERR_DATE                 37
#define PM_ERR_TIME                 38
#define PM_ERR_COUNT                39

#define ERRCODE(x)                  (1L<<x)
#define ERRNULL                     -1

#define PM_SNAME_POS                0
#define PM_REC_ID_POS               1
#define PM_YEAR_POS                 2

/*#define PM_TT_OFFSET                CP_HEADER*/
#define PM_TT_POS                   0
#define PM_TT_A_POS                 1
#define PM_TT_D_POS                 2

/*#define PM_CH_A_OFFSET              TT_OFFSET + CP_TT*/
#define PM_AN_POS                   0
#define PM_CHID_POS                 1
#define PM_PH_POS                   2
#define PM_CCBM_POS                 3
#define PM_UU_POS                   4
#define PM_A_POS                    5
#define PM_B_POS                    6
#define PM_SKEW_POS                 7
#define PM_MIN_POS                  8
#define PM_MAX_POS                  9
#define PM_PRIM_POS                 10
#define PM_SEC_POS                  11
#define PM_PS_POS                   12

/*#define PM_CH_D_OFFSET              PM_CH_A_OFFSET + CP_AN_1999*/
#define PM_DN_POS                   0
#define PM_Y_1991_POS               2
#define PM_Y_1999_POS               4

/*#define PM_LF_OFFSET                PM_CH_D_OFFSET + CP_DN_1999*/
#define PM_LF_POS                   0

/*#define PM_NRATES_OFFSET            PM_LF_OFFSET + CP_LF*/
#define PM_NRATES_POS               0

/*#define PM_SAMP_OFFSET              PM_NRATES_OFFSET + CP_NRATES*/
#define PM_SAMP_POS                 0
#define PM_ENDSAMP_POS              1

/*#define PM_TIME_OFFSET              PM_SAMP_OFFSET + CP_DATE_TIME*/
#define PM_DATE_POS                 0
#define PM_TIME_POS                 1

#define PM_DAY_POS                  0
#define PM_MON_POS                  1
#define PM_YEAR_POS                 2

#define PM_HOUR_POS                 0
#define PM_MINUT_POS                1
#define PM_SECONDS_POS              2

#define PM_SECONDS_P_POS            0
#define PM_SECONDS_S_POS            1

#define PM_FILETYPE_POS             0

#define PM_TIMEMULT_POS             0

#define PM_TC_POS                   0
#define PM_LC_POS                   1

#define PM_TMQ_POS                  0
#define PM_LEAPSEC_POS              1

#if 0
#define LN_ERR_MSK                  (LN_ERR_NOCR | LN_ERR_EXTRA_SPACES |\
                                    LN_ERR_TOO_MANY_PARAM |\
                                    LN_ERR_TOO_FEW_PARAM |\
                                    LN_ERR_INCORRECT_PARAM)
#endif

enum file_format {ascii, binary, binary32, float32, undef};
enum param_type {
    ptstring,   /* simple string */ 
    ptchar,     /* single char */ 
    ptcharhex,  /* hex code char */ 
    ptint,      /* integer */ 
    ptintc,     /* int + char */ 
    ptfloat,    /* float */ 
    ptdate,     /* date (format XX or XXXX) */
    pttime,     /* time (format XX) */
    ptstime, 
    ptsecond
};

enum revision {
    rev1991 = 1991,
    rev1999 = 1999,
    rev2001 = 2001,
    rev2013 = 2013 
};

enum cfg_pnum { psname, prec_id, prev_year, ptt, ptt_a, ptt_d, pan, pach_id,
    paphase, paccbm, pauu, paa, pab, paskew, pamin, pamax, paprimary,
    pasecondary, paps, pdn, pdch_id, pdphase, pdccbm, pdy_1991, pdy_1999,
    plf, pnrates, psamp, pendsamp, pstdate, psttime, pday, pmon, pyear, phours,
    pminuts, pseconds, pseconds_p, pseconds_s, pfiletype, ptimemult, ptimecode,
    plocalcode, ptmqcode, pleapsec };

typedef struct {
    int index;
    int ch_index;
    int len;
    long int val_int;
    double val_float;
    int err;
} cfg_pm_t;

typedef struct {
    int num;
    char *ch_id;
    char *phase;
    char *ccbm;
    char *uu;
    double a;
    double b;
    double skew;
    double min;
    double max;
    double primary;
    double secondary;
    char ps;
} cmtrd_an_t;

typedef struct {
    int num;
    char *ch_id;
    char *phase;
    char *ccbm;
    int y;
} cmtrd_dn_t;

typedef struct {
    double samp;
    long int end_samp;
} cmtrd_samp_t;

typedef struct {
    int day;
    int mon;
    int year;
    int hour;
    int min;
    int sec;
    int subsec;
} cmtrd_timestamp_t;

typedef struct {
    int ln;
    int strerr;
    long int paramerr;
} cmtrd_err_t;

typedef struct {
    char *station_name;
    char *rec_dev_id;
    int rev_year;
    int ch_count;
    int an_count;
    int dn_count;
    cmtrd_an_t *anv;
    cmtrd_dn_t *dnv;
    double frequency;
    int nrates;
    int real_nrates;
    cmtrd_samp_t *samps;
    cmtrd_timestamp_t start_datetime; 
    cmtrd_timestamp_t trig_datetime; 
    enum file_format ft;
    double timemult;
    char *time_code;
    char *local_code;
    int tmq_code;
    int leapsec;
    int errcount;
    int lastline;
    int last_correctline;
    cmtrd_err_t *errors;
} cmtrd_cfg_t;

#endif
