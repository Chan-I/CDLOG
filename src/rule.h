/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

/**
 * @file rule.h
 * @brief rule decide to output in format by category & level
 */

#ifndef __cdlog_rule_h
#define __cdlog_rule_h

#include <stdio.h>
#include <pthread.h>

#include "zc_defs.h"
#include "format.h"
#include "thread.h"
#include "rotater.h"
#include "record.h"

#ifdef __LOG_REP_RSTFL__
#define RESTFUL_METHOD_GET 1
#define RESTFUL_METHOD_POST 2
#define RESTFUL_METHOD_PUT 3
#define RESTFUL_METHOD_DELETE 4
typedef enum restful_method {
	get = 1,
	post = 2,
	put = 3,
	delete = 4
} restful_method;
#endif
typedef struct cdlog_rule_s cdlog_rule_t;

typedef int (*cdlog_rule_output_fn) (cdlog_rule_t * a_rule, cdlog_thread_t * a_thread);

struct cdlog_rule_s {
	char category[MAXLEN_CFG_LINE + 1];
	char compare_char;
	/* 
	 * [*] log all level
	 * [.] log level >= rule level, default
	 * [=] log level == rule level 
	 * [!] log level != rule level
	 */
	int level;
	unsigned char level_bitmap[32]; /* for category determine whether ouput or not */

	unsigned int file_perms;
	int file_open_flags;
	
#ifdef __cdlog_rule_h
	int need_restful;
	restful_method rst_mtd;
	char restful_address[MAXLEN_PATH + 1];
#endif

	char file_path[MAXLEN_PATH + 1];
	zc_arraylist_t *dynamic_specs;
	int static_fd;
	dev_t static_dev;
	ino_t static_ino;

	long archive_max_size;
	int archive_max_count;
	char archive_path[MAXLEN_PATH + 1];
	zc_arraylist_t *archive_specs;

	FILE *pipe_fp;
	int pipe_fd;

	size_t fsync_period;
	size_t fsync_count;

	zc_arraylist_t *levels;
	int syslog_facility;

	cdlog_format_t *format;
	cdlog_rule_output_fn output;

	char record_name[MAXLEN_PATH + 1];
	char record_path[MAXLEN_PATH + 1];
	cdlog_record_fn record_func;
};

cdlog_rule_t *cdlog_rule_new(char * line,
		zc_arraylist_t * levels,
		cdlog_format_t * default_format,
		zc_arraylist_t * formats,
		unsigned int file_perms,
		size_t fsync_period,
		int * time_cache_count);

void cdlog_rule_del(cdlog_rule_t * a_rule);
void cdlog_rule_profile(cdlog_rule_t * a_rule, int flag);
int cdlog_rule_match_category(cdlog_rule_t * a_rule, char *category);
int cdlog_rule_is_wastebin(cdlog_rule_t * a_rule);
int cdlog_rule_set_record(cdlog_rule_t * a_rule, zc_hashtable_t *records);
int cdlog_rule_output(cdlog_rule_t * a_rule, cdlog_thread_t * a_thread);

#endif
