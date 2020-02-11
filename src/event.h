/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_event_h
#define __cdlog_event_h

#include <sys/types.h>  /* for pid_t */
#include <sys/time.h>   /* for struct timeval */
#include <pthread.h>    /* for pthread_t */
#include <stdarg.h>     /* for va_list */
#include "zc_defs.h"

typedef enum {
	ZLOG_FMT = 0,
	ZLOG_HEX = 1,
} cdlog_event_cmd;

typedef struct cdlog_time_cache_s {
	char str[MAXLEN_CFG_LINE + 1];
	size_t len;
	time_t sec;
} cdlog_time_cache_t;

typedef struct {
	char *category_name;
	size_t category_name_len;
	char host_name[256 + 1];
	size_t host_name_len;

	const char *file;
	size_t file_len;
	const char *func;
	size_t func_len;
	long line;
	int level;

	const void *hex_buf;
	size_t hex_buf_len;
	const char *str_format;
	va_list str_args;
	cdlog_event_cmd generate_cmd;

	struct timeval time_stamp;

	time_t time_local_sec;
	struct tm time_local;

	cdlog_time_cache_t *time_caches;
	int time_cache_count;

	pid_t pid;
	pid_t last_pid;
	char pid_str[30 + 1];
	size_t pid_str_len;

	pthread_t tid;
	char tid_str[30 + 1];
	size_t tid_str_len;

	char tid_hex_str[30 + 1];
	size_t tid_hex_str_len;

#if defined __linux__ || __APPLE__
	pid_t ktid;
	char ktid_str[30+1];
	size_t ktid_str_len;
#endif
} cdlog_event_t;


cdlog_event_t *cdlog_event_new(int time_cache_count);
void cdlog_event_del(cdlog_event_t * a_event);
void cdlog_event_profile(cdlog_event_t * a_event, int flag);

void cdlog_event_set_fmt(cdlog_event_t * a_event,
			char *category_name, size_t category_name_len,
			const char *file, size_t file_len, const char *func, size_t func_len, long line, int level,
			const char *str_format, va_list str_args);

void cdlog_event_set_hex(cdlog_event_t * a_event,
			char *category_name, size_t category_name_len,
			const char *file, size_t file_len, const char *func, size_t func_len, long line, int level,
			const void *hex_buf, size_t hex_buf_len);

#endif
