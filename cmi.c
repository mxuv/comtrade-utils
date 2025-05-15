#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmi_msg.h"
#include "format.h"
#include "cmd.h"
#include "cfg.h"

void print_nchar(char c, int n)
{
    int i;
    for (i = 0; i < n; i++)
        putc(c, stdout);
}

int get_len_of_most_long_parammsg()
{
    int res = 0, tmp = 0, i;
    for (i = 0; i < PARAM_MSG_COUNT; i++) {
        tmp = strlen(parammsg[i]);
        if (tmp > res)
            res = tmp;
    }
    return res;
}

void print_msgs(int nsp_bf, int nmsg, int *spaces)
{
    print_nchar(' ', nsp_bf);
    printf("%s:", parammsg[nmsg]);
    print_nchar(' ', spaces[nmsg]);
    putc(' ', stdout);
}

void print_errors(cmtrd_cfg_t *cfg_rec)
{
    int i;
    if (!cfg_rec->errors) {
        fputs("No errors\n", stdout);
        return;
    }

    fputs("The following errors were found:\n", stdout);
    for (i = 0; i < cfg_rec->errcount; i++) {
        int msg_index;

        printf("line %d:\n", (cfg_rec->errors + i)->ln + 1);
        printf("  line errors:\n");
        for (msg_index = 0; msg_index < LN_ERR_COUNT; msg_index++) {
            int j = 1;
            if ((cfg_rec->errors + i)->strerr & (j << msg_index))
                printf("    %s\n", lnerrmsg[msg_index]);
        }
        if ((cfg_rec->errors + i)->paramerr) {
            long int j = 1;
            printf("  parameters with errors:\n");
            for (msg_index = 0; msg_index < PM_ERR_COUNT; msg_index++) {
                if ((cfg_rec->errors + i)->paramerr & (j << msg_index))
                    printf("    %s\n", parammsg[msg_index]);
            }
        }
    }
}

void print_achannels_info(cmtrd_cfg_t *cfg_rec, int *spaces)
{
    int i;
    for (i = 0; i < cfg_rec->an_count; i++) {
        printf("Analog channel:\n");
        print_msgs(2, 6, spaces);
        printf("%d\n",(cfg_rec->anv + i)->num);
        print_msgs(2, 7, spaces);
        printf("%s\n",(cfg_rec->anv + i)->ch_id);
        print_msgs(2, 8, spaces);
        printf("%s\n",(cfg_rec->anv + i)->phase);
        print_msgs(2, 9, spaces);
        printf("%s\n",(cfg_rec->anv + i)->ccbm);
        print_msgs(2, 10, spaces);
        printf("%s\n",(cfg_rec->anv + i)->uu);
        print_msgs(2, 11, spaces);
        printf("%lf\n",(cfg_rec->anv + i)->a);
        print_msgs(2, 12, spaces);
        printf("%lf\n",(cfg_rec->anv + i)->b);
        print_msgs(2, 13, spaces);
        printf("%lf\n",(cfg_rec->anv + i)->skew);
        print_msgs(2, 14, spaces);
        printf("%lf\n",(cfg_rec->anv + i)->min);
        print_msgs(2, 15, spaces);
        printf("%lf\n",(cfg_rec->anv + i)->max);
        print_msgs(2, 16, spaces);
        printf("%lf\n",(cfg_rec->anv + i)->primary);
        print_msgs(2, 17, spaces);
        printf("%lf\n",(cfg_rec->anv + i)->secondary);
        print_msgs(2, 18, spaces);
        printf("%c\n",(cfg_rec->anv + i)->ps);

    }
    fputs("\n", stdout);
}

void print_dchannels_info(cmtrd_cfg_t *cfg_rec, int *spaces)
{
    int i;
    for (i = 0; i < cfg_rec->dn_count; i++) {
        printf("Digital channel:\n");
        print_msgs(2, 19, spaces);
        printf("%d\n", (cfg_rec->dnv + i)->num);
        print_msgs(2, 7, spaces);
        printf("%s\n", (cfg_rec->dnv + i)->ch_id);
        print_msgs(2, 8, spaces);
        printf("%s\n", (cfg_rec->dnv + i)->phase);
        print_msgs(2, 9, spaces);
        printf("%s\n", (cfg_rec->dnv + i)->ccbm);
        print_msgs(2, 20, spaces);
        printf("%d\n", (cfg_rec->dnv + i)->y);

    }
    fputs("\n", stdout);
}

void print_general_info(cmtrd_cfg_t *cfg_rec, int *spaces)
{
    int i;
    printf("General info:\n");
    print_msgs(2, 0, spaces);
    printf("%s\n",cfg_rec->station_name);
    print_msgs(2, 1, spaces);
    printf("%s\n", cfg_rec->rec_dev_id);
    print_msgs(2, 2, spaces);
    printf("%d\n",cfg_rec->rev_year);
    print_msgs(2, 3, spaces);
    printf("%d\n",cfg_rec->ch_count);
    print_msgs(2, 4, spaces);
    printf("%d\n",cfg_rec->an_count);
    print_msgs(2, 5, spaces);
    printf("%d\n",cfg_rec->dn_count);
    print_msgs(2, 21, spaces);
    printf("%lf\n",cfg_rec->frequency);
    print_msgs(2, 22, spaces);
    printf("%d\n",cfg_rec->nrates);
    for (i = 0; i < cfg_rec->real_nrates; i++) {
        print_msgs(2, 23, spaces);
        printf("%lf\n", (cfg_rec->samps + i)->samp);
        print_msgs(2, 24, spaces);
        printf("%ld\n", (cfg_rec->samps + i)->end_samp);
    }
    print_msgs(2, 39, spaces);
    printf("%02d/%02d/%d %02d:%02d:%02d.%d\n", 
            cfg_rec->start_datetime.day, cfg_rec->start_datetime.mon,
            cfg_rec->start_datetime.year, cfg_rec->start_datetime.hour,
            cfg_rec->start_datetime.min, cfg_rec->start_datetime.sec,
            cfg_rec->start_datetime.subsec);
    print_msgs(2, 40, spaces);
    printf("%02d/%02d/%d %02d:%02d:%02d.%d\n", 
            cfg_rec->trig_datetime.day, cfg_rec->trig_datetime.mon,
            cfg_rec->trig_datetime.year, cfg_rec->trig_datetime.hour,
            cfg_rec->trig_datetime.min, cfg_rec->trig_datetime.sec,
            cfg_rec->trig_datetime.subsec);
    print_msgs(2, 31, spaces);
    if (cfg_rec->ft == undef)
        printf("Undefined\n");
    else
        printf("%s\n", ffv[cfg_rec->ft]);

    if (cfg_rec->rev_year >= rev1999) {
        print_msgs(2, 32, spaces);
        printf("%lf\n", cfg_rec->timemult);
    }
    if (cfg_rec->rev_year == rev2013) {
        print_msgs(2, 33, spaces);
        printf("%s\n", cfg_rec->time_code);
        print_msgs(2, 34, spaces);
        printf("%s\n",cfg_rec->local_code);
        print_msgs(2, 35, spaces);
        printf("%d\n",cfg_rec->tmq_code);
        print_msgs(2, 36, spaces);
        printf("%d\n",cfg_rec->leapsec);
    }
    fputs("\n", stdout);
}

void init_array_of_spaces(int *arr)
{
    int i, maxlen;

    maxlen = get_len_of_most_long_parammsg();
    for (i = 0; i < PARAM_MSG_COUNT; i++)
        arr[i] = maxlen - strlen(parammsg[i]);
}

void print_info(cmtrd_cfg_t *cfg_rec, int opts)
{
    int spaces[PARAM_MSG_COUNT];

    init_array_of_spaces(spaces);
    if (0 == opts)
        print_general_info(cfg_rec, spaces);

    if (opts & OPT_ALL) {
        print_general_info(cfg_rec, spaces);
        print_achannels_info(cfg_rec, spaces);
        print_dchannels_info(cfg_rec, spaces);
        print_errors(cfg_rec);
        return;
    }

    if (opts & OPT_ERRORS_ONLY) {
        print_errors(cfg_rec);
        return;
    }

    if (opts & OPT_SHORT_INFO)
        print_general_info(cfg_rec, spaces);

    if (opts & OPT_CH_INFO_ONLY) {
        print_achannels_info(cfg_rec, spaces);
        print_dchannels_info(cfg_rec, spaces);
    }

    if (!(opts & OPT_NOERRORS))
        print_errors(cfg_rec);
}

void print_help()
{
    fputs(
        "comtrade-info - printing information about COMTRADE files \n"
        "\n"
        "Usage: comtrade-info [options] [input file]\n"
        "\n"
        "Options:\n"
        "  -a   show full information\n"
        "  -c   show info about channels only\n"
        "  -e   show errors only\n"
        "  -E   dont' show errors\n"
        "  -i   print short info\n"
        "  -h   show this help\n"
        "  -v   show version\n", stdout);
}

void print_version()
{
    fputs("Version\n", stdout);
}

void print_noinput_file()
{
    fputs(
        "No input file\n"
        "Nothing to do\n", stderr);
}

void print_incorrect_opt(const char *str)
{
    fprintf(stderr, "Incorrect option \"%s\"\n"
        "Try -h for help\n", str);
}

struct cmd_opts {
    int options;
    char *cfg_fname;
};

void opts_init(struct cmd_opts *opts)
{
    memset(opts, 0, sizeof(*opts));
    /* opts->options = OPT_SHORT_INFO; */
}

int parsing_opts(int argc, char **argv, struct cmd_opts *opts)
{
    int nopt = 1;

    if (argc < 2) {
        print_noinput_file();
        return 1;
    }
    while (nopt < argc) {
        if (*argv[nopt] == '-') {
            if (strlen(argv[nopt]) > 2) {
                print_incorrect_opt(argv[nopt]);
                return 1;
            }
            switch (argv[nopt][1]) {
            case 'a':
                opts->options |= OPT_ALL;
                break;
            case 'c':
                opts->options |= OPT_CH_INFO_ONLY;
                break;
            case 'e':
                opts->options |= OPT_ERRORS_ONLY;
                break;
            case 'E':
                opts->options |= OPT_NOERRORS;
                break;
            case 'h':
                print_help();
                exit(0);
            case 'i':
                opts->options |= OPT_SHORT_INFO;
                break;
            case 'v':
                print_version();
                exit(0);
            default:
                print_incorrect_opt(argv[nopt]);
                return 1;
            }
        } else {
            opts->cfg_fname = argv[nopt];
        }
        nopt++;
    }
    if (opts->cfg_fname == NULL) {
        print_noinput_file();
        return 1;
    }
    return 0;
}

/* return codes:
 * 0 - Ok
 * 1 - Incorrect cmd options
 * 2 - Analysis file err
 * 3 - Unexcepted end of file
 * 5 - Malloc error
*/
int main(int argc, char **argv)
{
    int res;
    FILE *fd;
    cmtrd_cfg_t cfg_rec;
    struct cmd_opts opts;

    opts_init(&opts);
    res = parsing_opts(argc, argv, &opts);
    if (res)
        return 1;

    fd = fopen(opts.cfg_fname, "rb");
    if (fd == NULL) {
        perror(argv[1]);
        return 1;
    }

    cfg_record_init(&cfg_rec);
    res = analyze_cfgfile(fd, &cfg_rec);
    fclose(fd);
    if (res == 1) {
        fputs("Critical error. Analysis has been aborted.\n", stderr);
        print_errors(&cfg_rec);
        return 2;
    }

    if (res == 2) {
        fputs("Unexcepted end of file. Analysis has been aborted.\n", stderr);
        return 3;
    }

    print_info(&cfg_rec, opts.options);
    return 0;
} 
