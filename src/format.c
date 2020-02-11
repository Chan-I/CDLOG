/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "zc_defs.h"
#include "thread.h"
#include "spec.h"
#include "format.h"

void cdlog_format_profile(cdlog_format_t * a_format, int flag)
{

	zc_assert(a_format,);
	zc_profile(flag, "---format[%p][%s = %s(%p)]---",
		a_format,
		a_format->name,
		a_format->pattern,
		a_format->pattern_specs);

#if 0
	int i;
	cdlog_spec_t *a_spec;
	zc_arraylist_foreach(a_format->pattern_specs, i, a_spec) {
		cdlog_spec_profile(a_spec, flag);
	}
#endif

	return;
}

/*******************************************************************************/
void cdlog_format_del(cdlog_format_t * a_format)
{
	zc_assert(a_format,);
	if (a_format->pattern_specs) {
		zc_arraylist_del(a_format->pattern_specs);
	}
	zc_debug("cdlog_format_del[%p]", a_format);
    free(a_format);
	return;
}

cdlog_format_t *cdlog_format_new(char *line, int * time_cache_count)
{
	int nscan = 0;
	cdlog_format_t *a_format = NULL;
	int nread = 0;
	const char *p_start;
	const char *p_end;
	char *p;
	char *q;
	cdlog_spec_t *a_spec;

	zc_assert(line, NULL);

	a_format = calloc(1, sizeof(cdlog_format_t));
	if (!a_format) {
		zc_error("calloc fail, errno[%d]", errno);
		return NULL;
	}

	/* line         default = "%d(%F %X.%l) %-6V (%c:%F:%L) - %m%n"
	 * name         default
	 * pattern      %d(%F %X.%l) %-6V (%c:%F:%L) - %m%n
	 */
	memset(a_format->name, 0x00, sizeof(a_format->name));
	nread = 0;
	nscan = sscanf(line, " %[^= \t] = %n", a_format->name, &nread);
	if (nscan != 1) {
		zc_error("format[%s], syntax wrong", line);
		goto err;
	}

	if (*(line + nread) != '"') {
		zc_error("the 1st char of pattern is not \", line+nread[%s]", line+nread);
		goto err;
	}

	for (p = a_format->name; *p != '\0'; p++) {
		if ((!isalnum(*p)) && (*p != '_')) {
			zc_error("a_format->name[%s] character is not in [a-Z][0-9][_]", a_format->name);
			goto err;
		}
	}

	p_start = line + nread + 1;
	p_end = strrchr(p_start, '"');
	if (!p_end) {
		zc_error("there is no \" at end of pattern, line[%s]", line);
		goto err;
	}

	if (p_end - p_start > sizeof(a_format->pattern) - 1) {
		zc_error("pattern is too long");
		goto err;
	}
	memset(a_format->pattern, 0x00, sizeof(a_format->pattern));
	memcpy(a_format->pattern, p_start, p_end - p_start);

	if (zc_str_replace_env(a_format->pattern, sizeof(a_format->pattern))) {
		zc_error("zc_str_replace_env fail");
		goto err;
	}

	a_format->pattern_specs =
	    zc_arraylist_new((zc_arraylist_del_fn) cdlog_spec_del);
	if (!(a_format->pattern_specs)) {
		zc_error("zc_arraylist_new fail");
		goto err;
	}

	for (p = a_format->pattern; *p != '\0'; p = q) {
		a_spec = cdlog_spec_new(p, &q, time_cache_count);
		if (!a_spec) {
			zc_error("cdlog_spec_new fail");
			goto err;
		}

		if (zc_arraylist_add(a_format->pattern_specs, a_spec)) {
			cdlog_spec_del(a_spec);
			zc_error("zc_arraylist_add fail");
			goto err;
		}
	}

	cdlog_format_profile(a_format, ZC_DEBUG);
	return a_format;
err:
	cdlog_format_del(a_format);
	return NULL;
}

/*******************************************************************************/
/* return 0	success, or buf is full
 * return -1	fail
 */
int cdlog_format_gen_msg(cdlog_format_t * a_format, cdlog_thread_t * a_thread)
{
	int i;
	cdlog_spec_t *a_spec;

	cdlog_buf_restart(a_thread->msg_buf);

	zc_arraylist_foreach(a_format->pattern_specs, i, a_spec) {
		if (cdlog_spec_gen_msg(a_spec, a_thread) == 0) {
			continue;
		} else {
			return -1;
		}
	}

	return 0;
}
