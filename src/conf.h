/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_conf_h
#define __cdlog_conf_h

#include "zc_defs.h"
#include "format.h"
#include "rotater.h"

typedef struct cdlog_conf_s {
	char file[MAXLEN_PATH + 1];
	char mtime[20 + 1];

	int strict_init;
	size_t buf_size_min;
	size_t buf_size_max;

	char rotate_lock_file[MAXLEN_CFG_LINE + 1];
	cdlog_rotater_t *rotater;

	char default_format_line[MAXLEN_CFG_LINE + 1];
	cdlog_format_t *default_format;

	unsigned int file_perms;
	size_t fsync_period;
	size_t reload_conf_period;

	zc_arraylist_t *levels;
	zc_arraylist_t *formats;
	zc_arraylist_t *rules;
	int time_cache_count;
} cdlog_conf_t;

extern cdlog_conf_t * cdlog_env_conf;

cdlog_conf_t *cdlog_conf_new(const char *confpath);
void cdlog_conf_del(cdlog_conf_t * a_conf);
void cdlog_conf_profile(cdlog_conf_t * a_conf, int flag);

#endif
