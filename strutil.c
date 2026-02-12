#include "strutil.h"

int parsing_tokens(const char *str, int len, char delim, struct token *tks)
{
    int i, start = 0, count = 0;

    if (len <= 0)
        return 0;

    for (i = 0; i <= len; i++) {
        if (i == len || str[i] == delim) {
            tks[count].tok_start = start;
            tks[count].tok_len = i - start;
            start = i + 1;
            count++;
        }
    }
    return count;
}

int delete_crlf(char *str, int strlen)
{
    int newlen = strlen;

    if (strlen <= 0)
        return 0;

    if (strlen >= 2 && str[strlen-2] == '\r' && str[strlen-1] == '\n')
        newlen -= 2;
    else if (str[strlen-1] == '\r' || str[strlen-1] == '\n')
        newlen -= 1;

    str[newlen] = '\0';

    return newlen;
}

