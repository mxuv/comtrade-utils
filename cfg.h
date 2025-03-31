#ifndef _CFG_H_
#define _CFG_H_

#include "format.h"

extern const char *ffv[];

void cfg_record_init(cmtrd_cfg_t *cfg_rec);
int analyze_cfgfile(FILE *fd, cmtrd_cfg_t *cfg_rec);

#endif
