/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_format_h
#define __cdlog_format_h

#include "thread.h"
#include "zc_defs.h"

typedef struct cdlog_format_s cdlog_format_t;

struct cdlog_format_s {
	char name[MAXLEN_CFG_LINE + 1];	
	char pattern[MAXLEN_CFG_LINE + 1];
	zc_arraylist_t *pattern_specs;
};

cdlog_format_t *cdlog_format_new(char *line, int * time_cache_count);
void cdlog_format_del(cdlog_format_t * a_format);
void cdlog_format_profile(cdlog_format_t * a_format, int flag);

int cdlog_format_gen_msg(cdlog_format_t * a_format, cdlog_thread_t * a_thread);

#define cdlog_format_has_name(a_format, fname) \
	STRCMP(a_format->name, ==, fname)

#endif
