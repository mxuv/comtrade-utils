#ifndef _PARS_H_
#define _PARS_H_


int is_line_ending_ok(const char *str, int len);
int get_param_count(const char *str, char separator, int len);
int get_param_index(const char *str, int param, char separator);
int is_correct_param_length(int len, int min, int max);
int get_param_length(const char *str, int stringlen, int param,
    int param_count, char separator);

#endif
