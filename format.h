#ifndef _FORMAT_H_
#define _FORMAT_H_

#define STATION_NAME_LEN            64
#define REC_DEV_ID_LEN              64
#define REV_YEAR_LEN                4

#define TT_LEN                      6
#define AN_LEN                      7
#define DN_LEN                      7

#define A_N_LEN                     6
#define A_CH_ID_LEN                 128
#define A_PH_LEN                    2
#define A_CCBM_LEN                  64
#define A_A_LEN                     32
#define A_B_LEN                     32
#define A_SKEW_LEN                  32
#define A_MIN_LEN                   13
#define A_MAX_LEN                   13
#define A_PRIMARY_LEN               32
#define A_SECONDARY_LEN             32
#define A_PS_LEN                    2

#define D_N_LEN                     A_N_LEN
#define D_CH_ID_LEN                 A_CH_ID_LEN
#define D_PH_LEN                    A_PH_LEN
#define D_CCBM_LEN                  A_CCBM_LEN
#define D_Y_LEN                     1

#define LF_LEN                      32

#define NRATES_LEN                  3
#define SAMP_LEN                    32
#define ENDSAMP_LEN                 32

#define DAY_LEN                     2
#define MON_LEN                     2
#define YEAR_LEN                    4
#define HOUR_LEN                    2
#define SECONDS_M_LEN               9
#define SECONDS_N_LEN               12

#define FILE_TYPE_LEN               8

#define TIMEMULT_LEN                32

#define TIME_CODE_LEN               6
#define LOCAL_CODE_LEN              6

#define TMQ_CODE_LEN                1
#define LEAPSEC_LEN                 1 

enum scales_ind {none, primary, secondary};
enum file_format {ascii, binary, binary32, float32};

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
    enum scales_ind ps;
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
    int end_samp;
} cmtrd_samp_t;

typedef struct {
    int day;
    int mon
    int year;
    int hour;
    int min;
    int sec;
    int subsec;
} cmtrd_timestamp_t;

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
    cmtrd_samp_t *samps;
    cmtrd_timestamp_t timestamps;
    enum file_format ft;
    double timemult;
    char *time_code;
    char *local_code;
    int tmp_code;
    int leapsec;
} cmtrd_cfg_body_t;

#endif
