/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_level_h
#define __cdlog_level_h

#include "zc_defs.h"

typedef struct cdlog_level_s {
	int int_level;
	char str_uppercase[MAXLEN_PATH + 1];
	char str_lowercase[MAXLEN_PATH + 1];
	size_t str_len;
       	int syslog_level;
} cdlog_level_t;

cdlog_level_t *cdlog_level_new(char *line);
void cdlog_level_del(cdlog_level_t *a_level);
void cdlog_level_profile(cdlog_level_t *a_level, int flag);

#endif
