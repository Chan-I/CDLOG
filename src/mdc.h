/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __cdlog_mdc_h
#define __cdlog_mdc_h

#include "zc_defs.h"

typedef struct cdlog_mdc_s cdlog_mdc_t;
struct cdlog_mdc_s {
	zc_hashtable_t *tab;
};

cdlog_mdc_t *cdlog_mdc_new(void);
void cdlog_mdc_del(cdlog_mdc_t * a_mdc);
void cdlog_mdc_profile(cdlog_mdc_t *a_mdc, int flag);

void cdlog_mdc_clean(cdlog_mdc_t * a_mdc);
int cdlog_mdc_put(cdlog_mdc_t * a_mdc, const char *key, const char *value);
char *cdlog_mdc_get(cdlog_mdc_t * a_mdc, const char *key);
void cdlog_mdc_remove(cdlog_mdc_t * a_mdc, const char *key);

typedef struct cdlog_mdc_kv_s {
	char key[MAXLEN_PATH + 1];
	char value[MAXLEN_PATH + 1];
	size_t value_len;
} cdlog_mdc_kv_t;

cdlog_mdc_kv_t *cdlog_mdc_get_kv(cdlog_mdc_t * a_mdc, const char *key);

#endif
