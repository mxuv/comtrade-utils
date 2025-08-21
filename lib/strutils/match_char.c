#include "strutils.h"

int match_char(char ch, char patt)
{   
    if (ch == patt)
        return 1;
    else
        return 0;
} 
