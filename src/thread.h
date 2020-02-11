/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_thread_h
#define  __cdlog_thread_h

#include "zc_defs.h"
#include "event.h"
#include "buf.h"
#include "mdc.h"

typedef struct {
	int init_version;
	cdlog_mdc_t *mdc;
	cdlog_event_t *event;

	cdlog_buf_t *pre_path_buf;
	cdlog_buf_t *path_buf;
	cdlog_buf_t *archive_path_buf;
	cdlog_buf_t *pre_msg_buf;
	cdlog_buf_t *msg_buf;
} cdlog_thread_t;


void cdlog_thread_del(cdlog_thread_t * a_thread);
void cdlog_thread_profile(cdlog_thread_t * a_thread, int flag);
cdlog_thread_t *cdlog_thread_new(int init_version,
			size_t buf_size_min, size_t buf_size_max, int time_cache_count);

int cdlog_thread_rebuild_msg_buf(cdlog_thread_t * a_thread, size_t buf_size_min, size_t buf_size_max);
int cdlog_thread_rebuild_event(cdlog_thread_t * a_thread, int time_cache_count);

#endif
