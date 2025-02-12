#ifndef _FORMAT_H_
#define _FORMAT_H_

typedef struct {
} cmtrd_an_t;

typedef struct {
} cmtrd_dn_t;

typedef struct {
    double samp;
    int end_samp;
} cmtrd_samp_t;

typedef struct {
    char *station_name;
    char *rec_dev_id;
    int rev_year;
    int ch_count;
    int an_count;
    int dn_count;
    cmtrd_an_t **anv;
    cmtrd_dn_t **dnv;
    double frequency;
    int nrates;
    cmtrd_samp_t **samps;
} cmtrd_body_t;

#endif
