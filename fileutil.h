#ifndef __FILEUTIL_H__
#define __FILEUTIL_H__

enum getstring_status {
    gss_ok,
    gss_eof,
    gss_err,
    gss_overflow
};

int getstring(FILE *fd, char *buffer, int bufsize,
                enum getstring_status *status);

#endif
