/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_category_h
#define __cdlog_category_h

#include "zc_defs.h"
#include "thread.h"

typedef struct cdlog_category_s {
	char name[MAXLEN_PATH + 1];
	size_t name_len;
	unsigned char level_bitmap[32];
	unsigned char level_bitmap_backup[32];
	zc_arraylist_t *fit_rules;
	zc_arraylist_t *fit_rules_backup;
} cdlog_category_t;

cdlog_category_t *cdlog_category_new(const char *name, zc_arraylist_t * rules);
void cdlog_category_del(cdlog_category_t * a_category);
void cdlog_category_profile(cdlog_category_t *a_category, int flag);

int cdlog_category_update_rules(cdlog_category_t * a_category, zc_arraylist_t * new_rules);
void cdlog_category_commit_rules(cdlog_category_t * a_category);
void cdlog_category_rollback_rules(cdlog_category_t * a_category);

int cdlog_category_output(cdlog_category_t * a_category, cdlog_thread_t * a_thread);

#define cdlog_category_needless_level(a_category, lv) \
        !((a_category->level_bitmap[lv/8] >> (7 - lv % 8)) & 0x01)


#endif
