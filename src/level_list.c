/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include "syslog.h"

#include "zc_defs.h"
#include "level.h"
#include "level_list.h"

/* cdlog_level_list == zc_arraylist_t<cdlog_level_t> */

void cdlog_level_list_profile(zc_arraylist_t *levels, int flag)
{
	int i;
	cdlog_level_t *a_level;

	zc_assert(levels,);
	zc_profile(flag, "--level_list[%p]--", levels);
	zc_arraylist_foreach(levels, i, a_level) {
		/* skip empty slots */
		if (a_level) cdlog_level_profile(a_level, flag);
	}
	return;
}

/*******************************************************************************/
void cdlog_level_list_del(zc_arraylist_t *levels)
{
	zc_assert(levels,);
	zc_arraylist_del(levels);
	zc_debug("zc_level_list_del[%p]", levels);
	return;
}

static int cdlog_level_list_set_default(zc_arraylist_t *levels)
{
	return cdlog_level_list_set(levels, "* = 0, LOG_INFO")
	|| cdlog_level_list_set(levels, "DEBUG = 20, LOG_DEBUG")
	|| cdlog_level_list_set(levels, "INFO = 40, LOG_INFO")
	|| cdlog_level_list_set(levels, "NOTICE = 60, LOG_NOTICE")
	|| cdlog_level_list_set(levels, "WARN = 80, LOG_WARNING")
	|| cdlog_level_list_set(levels, "ERROR = 100, LOG_ERR")
	|| cdlog_level_list_set(levels, "FATAL = 120, LOG_ALERT")
	|| cdlog_level_list_set(levels, "UNKNOWN = 254, LOG_ERR")
	|| cdlog_level_list_set(levels, "! = 255, LOG_INFO");
}

zc_arraylist_t *cdlog_level_list_new(void)
{
	zc_arraylist_t *levels;

	levels = zc_arraylist_new((zc_arraylist_del_fn)cdlog_level_del);
	if (!levels) {
		zc_error("zc_arraylist_new fail");
		return NULL;
	}

	if (cdlog_level_list_set_default(levels)) {
		zc_error("cdlog_level_set_default fail");
		goto err;
	}

	//cdlog_level_list_profile(levels, ZC_DEBUG);
	return levels;
err:
	zc_arraylist_del(levels);
	return NULL;
}

/*******************************************************************************/
int cdlog_level_list_set(zc_arraylist_t *levels, char *line)
{
	cdlog_level_t *a_level;

	a_level = cdlog_level_new(line);
	if (!a_level) {
		zc_error("cdlog_level_new fail");
		return -1;
	}

	if (zc_arraylist_set(levels, a_level->int_level, a_level)) {
		zc_error("zc_arraylist_set fail");
		goto err;
	}

	return 0;
err:
	zc_error("line[%s]", line);
	cdlog_level_del(a_level);
	return -1;
}

cdlog_level_t *cdlog_level_list_get(zc_arraylist_t *levels, int l)
{
	cdlog_level_t *a_level;

#if 0
	if ((l <= 0) || (l > 254)) {
		/* illegal input from cdlog() */
		zc_error("l[%d] not in (0,254), set to UNKOWN", l);
		l = 254;
	}
#endif

	a_level = zc_arraylist_get(levels, l);
	if (a_level) {
		return a_level;
	} else {
		/* empty slot */
		zc_error("l[%d] not in (0,254), or has no level defined,"
			"see configure file define, set to UNKOWN", l);
		return zc_arraylist_get(levels, 254);
	}
}

/*******************************************************************************/

int cdlog_level_list_atoi(zc_arraylist_t *levels, char *str)
{
	int i;
	cdlog_level_t *a_level;

	if (str == NULL || *str == '\0') {
		zc_error("str is [%s], can't find level", str);
		return -1;
	}

	zc_arraylist_foreach(levels, i, a_level) {
		if (a_level && STRICMP(str, ==, a_level->str_uppercase)) {
			return i;
		}
	}

	zc_error("str[%s] can't found in level list", str);
	return -1;
}

