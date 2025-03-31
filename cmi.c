#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmi_msg.h"
#include "strutils.h"
#include "format.h"
#include "cmd.h"
#include "cfg.h"

void print_errors(cmtrd_cfg_t *cfg_rec)
{
    int i;
    if (!cfg_rec->errors) {
        fputs("\nNo errors\n", stdout);
        return;
    }

    fputs("\nThe following errors were found:\n", stdout);
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

void print_general_info(cmtrd_cfg_t *cfg_rec)
{
    int i;
    printf("General info:\n");
    printf("    %s: %s\n", parammsg[0], cfg_rec->station_name);
    printf("    %s: %s\n", parammsg[1], cfg_rec->rec_dev_id);
    printf("    %s: %d\n", parammsg[2], cfg_rec->rev_year);
    printf("    %s: %d\n", parammsg[3], cfg_rec->ch_count);
    printf("    %s: %d\n", parammsg[4], cfg_rec->an_count);
    printf("    %s: %d\n", parammsg[5], cfg_rec->dn_count);
    printf("    %s: %lf\n", parammsg[21], cfg_rec->frequency);
    printf("    %s: %d\n", parammsg[22], cfg_rec->nrates);
    for (i = 0; i < cfg_rec->real_nrates; i++) {
        printf("    %s: %lf\n", parammsg[23], (cfg_rec->samps + i)->samp);
        printf("    %s: %ld\n", parammsg[24], (cfg_rec->samps + i)->end_samp);
    }
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
    if (cfg_rec->ft == undef)
        printf("    %s: Undefined\n", parammsg[31]);
    else
        printf("    %s: %s\n", parammsg[31], ffv[cfg_rec->ft]);

    printf("    %s: %lf\n", parammsg[32], cfg_rec->timemult);
    printf("    %s: %s\n", parammsg[33], cfg_rec->time_code);
    printf("    %s: %s\n", parammsg[34], cfg_rec->local_code);
    printf("    %s: %d\n", parammsg[35], cfg_rec->tmq_code);
    printf("    %s: %d\n", parammsg[36], cfg_rec->leapsec);
}

void print_info(cmtrd_cfg_t *cfg_rec, int opts)
{
    if (0 == opts)

    if (opts & OPT_ALL) {
        print_general_info(cfg_rec);
        print_achannels_info(cfg_rec);
        print_dchannels_info(cfg_rec);
        print_errors(cfg_rec);
        return;
    }

    if (opts & OPT_ERRORS_ONLY) {
        print_errors(cfg_rec);
        return;
    }

    if (opts & OPT_CH_INFO_ONLY) {
        print_achannels_info(cfg_rec);
        print_dchannels_info(cfg_rec);
    }

    if (opts & OPT_SHORT_INFO)
        print_general_info(cfg_rec);

    if (!(opts & OPT_NOERRORS))
        print_errors(cfg_rec);
}

void print_help()
{
    fputs("Help\n", stdout);
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
 * 2 - Analysis file aborted
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

    fd = fopen(opts.cfg_fname, "r");
    if (fd == NULL) {
        perror(argv[1]);
        return 1;
    }

    cfg_record_init(&cfg_rec);
    res = analyze_cfgfile(fd, &cfg_rec);
    fclose(fd);
    if (res) {
        fputs("Critical error. Analysis has been aborted.\n", stderr);
        return 2;
    }

    print_info(&cfg_rec, opts.options);
    return 0;
} 
