/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_rotater_h
#define __cdlog_rotater_h

#include "zc_defs.h"

typedef struct cdlog_rotater_s {
	pthread_mutex_t lock_mutex;
	char *lock_file;
	int lock_fd;

	/* single-use members */
	char *base_path;			/* aa.log */
	char *archive_path;			/* aa.#5i.log */
	char glob_path[MAXLEN_PATH + 1];	/* aa.*.log */
	size_t num_start_len;			/* 3, offset to glob_path */
	size_t num_end_len;			/* 6, offset to glob_path */
	int num_width;				/* 5 */
	int mv_type;				/* ROLLING or SEQUENCE */
	int max_count;
	zc_arraylist_t *files;
} cdlog_rotater_t;

cdlog_rotater_t *cdlog_rotater_new(char *lock_file);
void cdlog_rotater_del(cdlog_rotater_t *a_rotater);

/*
 * return
 * -1	fail
 * 0	no rotate, or rotate and success
 */
int cdlog_rotater_rotate(cdlog_rotater_t *a_rotater,
		char *base_path, size_t msg_len,
		char *archive_path, long archive_max_size, int archive_max_count);

void cdlog_rotater_profile(cdlog_rotater_t *a_rotater, int flag);

#endif
