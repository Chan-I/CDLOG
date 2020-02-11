/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_spec_h
#define __cdlog_spec_h

#include "event.h"
#include "buf.h"
#include "thread.h"

typedef struct cdlog_spec_s cdlog_spec_t;

/* write buf, according to each spec's Conversion Characters */
typedef int (*cdlog_spec_write_fn) (cdlog_spec_t * a_spec,
			 	cdlog_thread_t * a_thread,
			 	cdlog_buf_t * a_buf);

/* gen a_thread->msg or gen a_thread->path by using write_fn */
typedef int (*cdlog_spec_gen_fn) (cdlog_spec_t * a_spec,
				cdlog_thread_t * a_thread);

struct cdlog_spec_s {
	char *str;
	int len;

	char time_fmt[MAXLEN_CFG_LINE + 1];
	int time_cache_index;
	char mdc_key[MAXLEN_PATH + 1];

	char print_fmt[MAXLEN_CFG_LINE + 1];
	int left_adjust;
	int left_fill_zeros;
	size_t max_width;
	size_t min_width;

	cdlog_spec_write_fn write_buf;
	cdlog_spec_gen_fn gen_msg;
	cdlog_spec_gen_fn gen_path;
	cdlog_spec_gen_fn gen_archive_path;
};

cdlog_spec_t *cdlog_spec_new(char *pattern_start, char **pattern_end, int * time_cache_count);
void cdlog_spec_del(cdlog_spec_t * a_spec);
void cdlog_spec_profile(cdlog_spec_t * a_spec, int flag);

#define cdlog_spec_gen_msg(a_spec, a_thread) \
	a_spec->gen_msg(a_spec, a_thread)

#define cdlog_spec_gen_path(a_spec, a_thread) \
	a_spec->gen_path(a_spec, a_thread)

#define cdlog_spec_gen_archive_path(a_spec, a_thread) \
	a_spec->gen_archive_path(a_spec, a_thread)

#endif
