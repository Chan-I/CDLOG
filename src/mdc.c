/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "mdc.h"
#include "zc_defs.h"

void cdlog_mdc_profile(cdlog_mdc_t *a_mdc, int flag)
{
	zc_hashtable_entry_t *a_entry;
	cdlog_mdc_kv_t *a_mdc_kv;

	zc_assert(a_mdc,);
	zc_profile(flag, "---mdc[%p]---", a_mdc);

	zc_hashtable_foreach(a_mdc->tab, a_entry) {
		a_mdc_kv = a_entry->value;
		zc_profile(flag, "----mdc_kv[%p][%s]-[%s]----",
				a_mdc_kv,
				a_mdc_kv->key, a_mdc_kv->value);
	}
	return;
}
/*******************************************************************************/
void cdlog_mdc_del(cdlog_mdc_t * a_mdc)
{
	zc_assert(a_mdc,);
	if (a_mdc->tab) zc_hashtable_del(a_mdc->tab);
	zc_debug("cdlog_mdc_del[%p]", a_mdc);
    free(a_mdc);
	return;
}

static void cdlog_mdc_kv_del(cdlog_mdc_kv_t * a_mdc_kv)
{
	zc_debug("cdlog_mdc_kv_del[%p]", a_mdc_kv);
    free(a_mdc_kv);
}

static cdlog_mdc_kv_t *cdlog_mdc_kv_new(const char *key, const char *value)
{
	cdlog_mdc_kv_t *a_mdc_kv;

	a_mdc_kv = calloc(1, sizeof(cdlog_mdc_kv_t));
	if (!a_mdc_kv) {
		zc_error("calloc fail, errno[%d]", errno);
		return NULL;
	}

	snprintf(a_mdc_kv->key, sizeof(a_mdc_kv->key), "%s", key);
	a_mdc_kv->value_len = snprintf(a_mdc_kv->value, sizeof(a_mdc_kv->value), "%s", value);
	return a_mdc_kv;
}

cdlog_mdc_t *cdlog_mdc_new(void)
{
	cdlog_mdc_t *a_mdc;

	a_mdc = calloc(1, sizeof(cdlog_mdc_t));
	if (!a_mdc) {
		zc_error("calloc fail, errno[%d]", errno);
		return NULL;
	}

	a_mdc->tab = zc_hashtable_new(20,
			      zc_hashtable_str_hash,
			      zc_hashtable_str_equal, NULL,
			      (zc_hashtable_del_fn) cdlog_mdc_kv_del);
	if (!a_mdc->tab) {
		zc_error("zc_hashtable_new fail");
		goto err;
	}

	//cdlog_mdc_profile(a_mdc, ZC_DEBUG);
	return a_mdc;
err:
	cdlog_mdc_del(a_mdc);
	return NULL;
}

/*******************************************************************************/
int cdlog_mdc_put(cdlog_mdc_t * a_mdc, const char *key, const char *value)
{
	cdlog_mdc_kv_t *a_mdc_kv;

	a_mdc_kv = cdlog_mdc_kv_new(key, value);
	if (!a_mdc_kv) {
		zc_error("cdlog_mdc_kv_new failed");
		return -1;
	}

	if (zc_hashtable_put(a_mdc->tab, a_mdc_kv->key, a_mdc_kv)) {
		zc_error("zc_hashtable_put fail");
		cdlog_mdc_kv_del(a_mdc_kv);
		return -1;
	}

	return 0;
}

void cdlog_mdc_clean(cdlog_mdc_t * a_mdc)
{
	zc_hashtable_clean(a_mdc->tab);
	return;
}

char *cdlog_mdc_get(cdlog_mdc_t * a_mdc, const char *key)
{
	cdlog_mdc_kv_t *a_mdc_kv;

	a_mdc_kv = zc_hashtable_get(a_mdc->tab, key);
	if (!a_mdc_kv) {
		zc_error("zc_hashtable_get fail");
		return NULL;
	} else {
		return a_mdc_kv->value;
	}
}

cdlog_mdc_kv_t *cdlog_mdc_get_kv(cdlog_mdc_t * a_mdc, const char *key)
{
	cdlog_mdc_kv_t *a_mdc_kv;

	a_mdc_kv = zc_hashtable_get(a_mdc->tab, key);
	if (!a_mdc_kv) {
		zc_error("zc_hashtable_get fail");
		return NULL;
	} else {
		return a_mdc_kv;
	}
}

void cdlog_mdc_remove(cdlog_mdc_t * a_mdc, const char *key)
{
	zc_hashtable_remove(a_mdc->tab, key);
	return;
}
