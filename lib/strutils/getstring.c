#include <string.h>

#include "strutils.h"

int getstring(FILE *fd, char *buffer, int bufsize,
                enum getstring_status *status)
{
    int len = 0;

    *status = gss_ok;
    if (fgets(buffer, bufsize, fd) != NULL) {
        len = strlen(buffer);
        if (buffer[len-1] != '\n') {
            if (len == bufsize - 1) {
                *status  = gss_overflow;
		return len;
	    }
        }
    } else {
	if (feof(fd))
	    *status = gss_eof;
	else
	    *status = gss_err;
    }

    return len;
}
