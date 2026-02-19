#ifndef __CFG_H__
#define __CFG_H__

enum data_format {ascii, binary, binary32, float32, undef};

typedef struct {
    double samp;
    long long int end_samp;
} cfg_samp_t;

typedef struct {
    int day;
    int mon;
    int year;
    int hour;
    int min;
    int sec;
    int subsec;
} cfg_timestamp_t;

typedef struct {
    char *station_name;
    char *device_id;
    int revision_year;
    int ch_cnt;
    int an_cnt;
    int dn_cnt;
    cmtrd_an_t *anv;
    cmtrd_dn_t *dnv;
    double lf;
    int nrates;
    cfg_samp_t *samps;
    cfg_timestamp_t *timestamps;
    int ft;
    double timemult;
    char *time_code;
    char *local_code;
    int tmq_code;
    int leapsec;
} cfg_body_t;

#endif
