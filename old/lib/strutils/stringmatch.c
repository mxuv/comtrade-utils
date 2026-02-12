#include "strutils.h"

int stringmatch(const char *str, const char *pattern)
{
    if (!*pattern)
        return *str == 0;
    if (*str != *pattern)
        return 0;
    else 
        return stringmatch(str + 1, pattern + 1);
}
