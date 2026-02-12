#include "strutils.h"

void stringcopy_c(char *dest, const char *src, int count)
{
    while (count) {
        *dest = *src;
        dest++;
        src++;
        count--;
    }

    *dest = 0;
}
