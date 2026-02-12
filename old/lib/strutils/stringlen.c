#include "strutils.h"

int stringlen(const char *str)
{
   const char *p;

    p = str;
    while (*p)
        p++;

    return p - str;
}
