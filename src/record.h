/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_record_h
#define __cdlog_record_h

#include "zc_defs.h"

/* record is user-defined output function and it's name from configure file */
typedef struct cdlog_msg_s {
	char *buf;
	size_t len;
	char *path;
} cdlog_msg_t; /* 3 of this first, see need thread or not later */

typedef int (*cdlog_record_fn)(cdlog_msg_t * msg);

typedef struct cdlog_record_s {
	char name[MAXLEN_PATH + 1];
	cdlog_record_fn output;
} cdlog_record_t;

cdlog_record_t *cdlog_record_new(const char *name, cdlog_record_fn output);
void cdlog_record_del(cdlog_record_t *a_record);
void cdlog_record_profile(cdlog_record_t *a_record, int flag);

#endif
