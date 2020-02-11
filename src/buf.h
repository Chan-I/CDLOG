/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_buf_h
#define __cdlog_buf_h

/* buf, is a dynamic expand buffer for one single log,
 * as one single log will interlace if use multiple write() to file.
 * and buf is always keep in a thread, to make each thread has its
 * own buffer to avoid lock.
 */

#include <stdarg.h>
#include <stdint.h>

typedef struct cdlog_buf_s {
	char *start;
	char *tail;
	char *end;
	char *end_plus_1;

	size_t size_min;
	size_t size_max;
	size_t size_real;

	char truncate_str[MAXLEN_PATH + 1];
	size_t truncate_str_len;
} cdlog_buf_t;


cdlog_buf_t *cdlog_buf_new(size_t min, size_t max, const char *truncate_str);
void cdlog_buf_del(cdlog_buf_t * a_buf);
void cdlog_buf_profile(cdlog_buf_t * a_buf, int flag);

int cdlog_buf_vprintf(cdlog_buf_t * a_buf, const char *format, va_list args);
int cdlog_buf_append(cdlog_buf_t * a_buf, const char *str, size_t str_len);
int cdlog_buf_adjust_append(cdlog_buf_t * a_buf, const char *str, size_t str_len,
			int left_adjust, int zero_pad, size_t in_width, size_t out_width);
int cdlog_buf_printf_dec32(cdlog_buf_t * a_buf, uint32_t ui32, int width);
int cdlog_buf_printf_dec64(cdlog_buf_t * a_buf, uint64_t ui64, int width);
int cdlog_buf_printf_hex(cdlog_buf_t * a_buf, uint32_t ui32, int width);

#define cdlog_buf_restart(a_buf) do { \
	a_buf->tail = a_buf->start; \
} while(0)

#define cdlog_buf_len(a_buf) (a_buf->tail - a_buf->start)
#define cdlog_buf_str(a_buf) (a_buf->start)
#define cdlog_buf_seal(a_buf) do {*(a_buf)->tail = '\0';} while (0)

#endif
