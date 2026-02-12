#ifndef __FORMAT_H__
#define __FORMAT_H__

/* token counts in line */
enum {
    tc_header_min       = 2,
    tc_header_max       = 3,
    tc_tt               = 3,
    tc_an1991           = 10,
    tc_an1999           = 13,
    tc_dn1991           = 3,
    tc_dn1999           = 5,
    tc_lf               = 1,
    tc_nrates           = 1,
    tc_samp             = 2,
    tc_timestamp        = 2,
    tc_triggtimestamp   = 2,
    tc_date             = 3,
    tc_time             = 3,
    tc_seconds          = 2,
    tc_filetype         = 1,
    tc_timemult         = 1,
    tc_timecode         = 2,
    tc_tmqcode          = 2
};

/* minimum and maximum tokens length */
enum {
    tl_sname_min        = 0,
    tl_sname_max        = 64,
    tl_recdev_min       = 0,
    tl_recdev_max       = 64,
    tl_revyear_min      = 4,
    tl_revyear_max      = 4,
    tl_tt_min           = 1,
    tl_tt_max           = 6,
    tl_an_min           = 2,
    tl_an_max           = 7,
    tl_dn_min           = 2,
    tl_dn_max           = 7,

#endif
