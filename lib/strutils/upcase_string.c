#include "strutils.h"

void upcase_string(char *str)
{
    while (*str) {
        *str = upcase_letter(*str);
        str++;
    }
}
