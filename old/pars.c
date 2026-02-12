#include "pars.h"

#include <strutils/strutils.h>

int is_line_ending_ok(const char *str, int len)
{
    if (str[len-2] == '\r')
        return 1;
    else
        return 0;
}

int get_param_count(const char *str, char separator, int len)
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

int get_param_index(const char *str, int param, char separator)
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

int get_param_length(const char *str, int stringlen, int param,
    int param_count, char separator)
{
    int index;

    index = get_param_index(str, param, separator);
    if ((param + 1) == param_count) {
            return stringlen - index;
    }
    else
        return get_param_index(str, param + 1, separator) - index - 1;
}

int is_correct_param_length(int len, int min, int max)
{
    if (len >= min && len <= max)
        return 1;
    else
        return 0;
}

