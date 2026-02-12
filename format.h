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

    tl_a_n_min          = 1,
    tl_a_n_max          = 6,
    tl_a_chid_min       = 1,
    tl_a_chid_max       = 128,
    tl_a_ph_min         = 0,
    tl_a_ph_max         = 2,
    tl_a_ccbm_min       = 0,
    tl_a_ccbm_max       = 64,
    tl_a_uu_min         = 1,
    tl_a_uu_max         = 32,
    tl_a_a_min          = 1,
    tl_a_a_max          = 32,
    tl_a_b_min          = 1,
    tl_a_b_max          = 32,
    tl_a_skew_min       = 1,
    tl_a_skew_max       = 32,
    tl_a_minsc_min      = 1,
    tl_a_minsc_max      = 13,
    tl_a_maxsc_min      = 1,
    tl_a_maxsc_max      = 13,
    tl_a_primary_min    = 1,
    tl_a_primary_max    = 32,
    tl_a_secondary_min  = 1,
    tl_a_secondary_max  = 32,
    tl_a_ps_min         = 1,
    tl_a_ps_max         = 1,

    tl_d_n_min          = 1,
    tl_d_n_max          = 6,
    tl_d_chid_min       = 1,
    tl_d_chid_max       = 128,
    tl_d_ph_min         = 0,
    tl_d_ph_max         = 2,
    tl_d_ccbm_min       = 0,
    tl_d_ccbm_max       = 64,
    tl_d_y_min          = 1,
    tl_d_y_max          = 1,

    tl_lf_min           = 0,
    tl_lf_max           = 32,

    tl_nrates_min       = 1,
    tl_nrates_max       = 32,


}

#endif
