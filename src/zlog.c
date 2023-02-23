/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include "fmacros.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>

#include "conf.h"
#include "category_table.h"
#include "record_table.h"
#include "mdc.h"
#include "zc_defs.h"
#include "rule.h"
#include "version.h"

/*******************************************************************************/
extern char *cdlog_git_sha1;
/*******************************************************************************/
static pthread_rwlock_t cdlog_env_lock = PTHREAD_RWLOCK_INITIALIZER;
cdlog_conf_t *cdlog_env_conf;
static pthread_key_t cdlog_thread_key;
static zc_hashtable_t *cdlog_env_categories;
static zc_hashtable_t *cdlog_env_records;
static cdlog_category_t *cdlog_default_category;
static size_t cdlog_env_reload_conf_count;
static int cdlog_env_is_init = 0;
static int cdlog_env_init_version = 0;
/*******************************************************************************/
/* inner no need thread-safe */
static void cdlog_fini_inner(void)
{
	/* pthread_key_delete(cdlog_thread_key); */
	/* never use pthread_key_delete,
	 * it will cause other thread can't release cdlog_thread_t 
	 * after one thread call pthread_key_delete
	 * also key not init will cause a core dump
	 */
	
	if (cdlog_env_categories) cdlog_category_table_del(cdlog_env_categories);
	cdlog_env_categories = NULL;
	cdlog_default_category = NULL;
	if (cdlog_env_records) cdlog_record_table_del(cdlog_env_records);
	cdlog_env_records = NULL;
	if (cdlog_env_conf) cdlog_conf_del(cdlog_env_conf);
	cdlog_env_conf = NULL;
	return;
}

static void cdlog_clean_rest_thread(void)
{
	cdlog_thread_t *a_thread;
	a_thread = pthread_getspecific(cdlog_thread_key);
	if (!a_thread) return;
	cdlog_thread_del(a_thread);
	return;
}

static int
cdlog_conf_parse_line_bj(int *section, char *line, cdlog_bj_conf_t *a_bj)
{
	int nscan;
	int nread;
	char name[MAXLEN_CFG_LINE + 1];
	char word_1[MAXLEN_CFG_LINE + 1];
	char word_2[MAXLEN_CFG_LINE + 1];
	char word_3[MAXLEN_CFG_LINE + 1];
	char value[MAXLEN_CFG_LINE + 1];

	if (strlen(line) > MAXLEN_CFG_LINE) {
		zc_error("line_len[%ld] > MAXLEN_CFG_LINE[%ld], may cause overflow",
			strlen(line), MAXLEN_CFG_LINE);
		return -1;
	}
	if (line[0] == '[') {
		int last_section = *section;
		nscan = sscanf(line, "[ %[^] \t]", name);
		if (STRCMP(name, ==, "global")) {
			*section = 1;
		} else if (STRCMP(name, ==, "classification")) {
			*section = 2;
		} else {
			zc_error("wrong section name[%s]", name);
			return -1;
		}
		if (last_section >= *section) {
			zc_error("wrong sequence of section, must follow global->levels->formats->rules");
			return -1;
		}
		return 0;
	}
	switch (*section) {
	case 1:
		memset(name, 0x00, sizeof(name));
		memset(value, 0x00, sizeof(value));
		nscan = sscanf(line, " %[^=]= %s ", name, value);
		printf("name:%s\nvalue:%s\n", name, value);
		break;
	case 2:

		break;
	default:

		break;
	}

	return 0;
}

static int
cdlog_conf_Beijing(char * confpath_bak, const char * confpath)
{
	/*
	 * 	confpath_bak	:	output file
	 *	confpath		:	intput file
	 */
	struct stat a_stat;
	cdlog_bj_conf_t a_bj;
	FILE *fp  = NULL;
	char line[MAXLEN_CFG_LINE + 1];
	size_t line_len;
	char *pline = NULL;
	char *p = NULL;
	int i = 0;
	int section = 0;
	int line_no = 0;
	int in_quotation = 0;

	/* global:1  classification:2 */

	if (lstat(confpath, &a_stat)) {
		zc_error("lstat conf file[%s] fail, errno[%d]", confpath);
		return -1;
	}
	if ((fp = fopen(confpath, "r")) == NULL) {
		zc_error("open configure file[%s] failed", confpath);
		return -1;
	}

	pline = line;
	memset(&line, 0x00, sizeof(line));
	while (fgets((char *)pline, sizeof(line) - (pline - line), fp) != NULL) {
		++line_no;
		line_len = strlen(pline);
		if (pline[line_len - 1] == '\n') {
			pline[line_len - 1] = '\0';
		}

		p = pline;
		while (*p && isspace((int) *p)) ++p;
		if (*p == '\0' || *p == '#') continue;

		for (i = 0; p[i] != '\0'; ++i) {
			pline[i] = p[i];
		}
		pline[i] = '\0';

		for (p = pline + strlen(pline) - 1; isspace((int)*p); ++p);

		if (*p == '\\') {
			if ((p - line) > MAXLEN_CFG_LINE - 30) {
				pline = line;
			} else {
				for (p--; isspace((int)*p); --p)
					;
				p++;
				*p = 0;
				pline = p;
				continue;
			}
		} else {
			pline = line;
		}

		*++p = '\0';

		in_quotation = 0;
		for (p = line; *p != '\0'; p++) {
			if (*p == '"') {
				in_quotation ^= 1;
				continue;
			}
			
			if (*p == '#' && !in_quotation) {
				*p = '0';
				break;
			}
		} //for
		//printf("line:%s\n", line);
		/*解析命令行内容*/
		
		if (-1 == cdlog_conf_parse_line_bj(&section, line, &a_bj)) {
			zc_error("cdlog_conf_parse_line_bj failed");
			return -1;
		}
	} //while

}



static int cdlog_init_inner(const char *confpath)
{
	int rc = 0;
	char confpath_bak[MAXLEN_PATH];
	/* the 1st time in the whole process do init */
	if (cdlog_env_init_version == 0) {
		/* clean up is done by OS when a thread call pthread_exit */
		rc = pthread_key_create(&cdlog_thread_key, (void (*) (void *)) cdlog_thread_del);
		if (rc) {
			zc_error("pthread_key_create fail, rc[%d]", rc);
			goto err;
		}

		/* if some thread do not call pthread_exit, like main thread
		 * atexit will clean it 
		 */
		rc = atexit(cdlog_clean_rest_thread);
		if (rc) {
			zc_error("atexit fail, rc[%d]", rc);
			goto err;
		}
		cdlog_env_init_version++;
	} /* else maybe after cdlog_fini() and need not create pthread_key */

	if (-1 == cdlog_conf_Beijing(confpath_bak, confpath)) {
		zc_error("cdlog_conf_Beijing failed!");
		goto err;
	}

	cdlog_env_conf = cdlog_conf_new(confpath_bak);
	if (!cdlog_env_conf) {
		zc_error("cdlog_conf_new[%s] fail", confpath_bak);
		goto err;
	}

	cdlog_env_categories = cdlog_category_table_new();
	if (!cdlog_env_categories) {
		zc_error("cdlog_category_table_new fail");
		goto err;
	}

	cdlog_env_records = cdlog_record_table_new();
	if (!cdlog_env_records) {
		zc_error("cdlog_record_table_new fail");
		goto err;
	}

	return 0;
err:
	cdlog_fini_inner();
	return -1;
}

/*******************************************************************************/
int cdlog_init(const char *confpath)
{
	int rc;
	zc_debug("------cdlog_init start------");
	zc_debug("------compile time[%s %s], version[%s]------", __DATE__, __TIME__, ZLOG_VERSION);

	rc = pthread_rwlock_wrlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return -1;
	}

	if (cdlog_env_is_init) {
		zc_error("already init, use cdlog_reload pls");
		goto err;
	}


	if (cdlog_init_inner(confpath)) {
		zc_error("cdlog_init_inner[%s] fail", confpath);
		goto err;
	}

	cdlog_env_is_init = 1;
	cdlog_env_init_version++;

	zc_debug("------cdlog_init success end------");
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return 0;
err:
	zc_error("------cdlog_init fail end------");
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return -1;
}

int dcdlog_init(const char *confpath, const char *cname)
{
	int rc = 0;
	zc_debug("------dcdlog_init start------");
	zc_debug("------compile time[%s %s], version[%s]------",
			__DATE__, __TIME__, ZLOG_VERSION);

	rc = pthread_rwlock_wrlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return -1;
	}

	if (cdlog_env_is_init) {
		zc_error("already init, use cdlog_reload pls");
		goto err;
	}

	if (cdlog_init_inner(confpath)) {
		zc_error("cdlog_init_inner[%s] fail", confpath);
		goto err;
	}

	cdlog_default_category = cdlog_category_table_fetch_category(
				cdlog_env_categories,
				cname,
				cdlog_env_conf->rules);
	if (!cdlog_default_category) {
		zc_error("cdlog_category_table_fetch_category[%s] fail", cname);
		goto err;
	}

	cdlog_env_is_init = 1;
	cdlog_env_init_version++;

	zc_debug("------dcdlog_init success end------");
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return 0;
err:
	zc_error("------dcdlog_init fail end------");
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return -1;
}
/*******************************************************************************/
int cdlog_reload(const char *confpath)
{
	int rc = 0;
	int i = 0;
	cdlog_conf_t *new_conf = NULL;
	cdlog_rule_t *a_rule;
	int c_up = 0;

	zc_debug("------cdlog_reload start------");
	rc = pthread_rwlock_wrlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return -1;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto quit;
	}

	/* use last conf file */
	if (confpath == NULL) confpath = cdlog_env_conf->file;

	/* reach reload period */
	if (confpath == (char*)-1) {
		/* test again, avoid other threads already reloaded */
		if (cdlog_env_reload_conf_count > cdlog_env_conf->reload_conf_period) {
			confpath = cdlog_env_conf->file;
		} else {
			/* do nothing, already done */
			goto quit;
		}
	}

	/* reset counter, whether automaticlly or mannually */
	cdlog_env_reload_conf_count = 0;

	new_conf = cdlog_conf_new(confpath);
	if (!new_conf) {
		zc_error("cdlog_conf_new fail");
		goto err;
	}

	zc_arraylist_foreach(new_conf->rules, i, a_rule) {
		cdlog_rule_set_record(a_rule, cdlog_env_records);
	}

	if (cdlog_category_table_update_rules(cdlog_env_categories, new_conf->rules)) {
		c_up = 0;
		zc_error("cdlog_category_table_update fail");
		goto err;
	} else {
		c_up = 1;
	}

	cdlog_env_init_version++;

	if (c_up) cdlog_category_table_commit_rules(cdlog_env_categories);
	cdlog_conf_del(cdlog_env_conf);
	cdlog_env_conf = new_conf;
	zc_debug("------cdlog_reload success, total init verison[%d] ------", cdlog_env_init_version);
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return 0;
err:
	/* fail, roll back everything */
	zc_warn("cdlog_reload fail, use old conf file, still working");
	if (new_conf) cdlog_conf_del(new_conf);
	if (c_up) cdlog_category_table_rollback_rules(cdlog_env_categories);
	zc_error("------cdlog_reload fail, total init version[%d] ------", cdlog_env_init_version);
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return -1;
quit:
	zc_debug("------cdlog_reload do nothing------");
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return 0;
}
/*******************************************************************************/
void cdlog_fini(void)
{
	int rc = 0;

	zc_debug("------cdlog_fini start------");
	rc = pthread_rwlock_wrlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return;
	}

	if (!cdlog_env_is_init) {
		zc_error("before finish, must cdlog_init() or dcdlog_init() fisrt");
		goto exit;
	}

	cdlog_fini_inner();
	cdlog_env_is_init = 0;

exit:
	zc_debug("------cdlog_fini end------");
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return;
	}
	return;
}
/*******************************************************************************/
cdlog_category_t *cdlog_get_category(const char *cname)
{
	int rc = 0;
	cdlog_category_t *a_category = NULL;

	zc_assert(cname, NULL);
	zc_debug("------cdlog_get_category[%s] start------", cname);
	rc = pthread_rwlock_wrlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return NULL;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		a_category = NULL;
		goto err;
	}

	a_category = cdlog_category_table_fetch_category(
				cdlog_env_categories,
				cname,
				cdlog_env_conf->rules);
	if (!a_category) {
		zc_error("cdlog_category_table_fetch_category[%s] fail", cname);
		goto err;
	}

	zc_debug("------cdlog_get_category[%s] success, end------ ", cname);
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return NULL;
	}
	return a_category;
err:
	zc_error("------cdlog_get_category[%s] fail, end------ ", cname);
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return NULL;
	}
	return NULL;
}

int dcdlog_set_category(const char *cname)
{
	int rc = 0;
	zc_assert(cname, -1);

	zc_debug("------dcdlog_set_category[%s] start------", cname);
	rc = pthread_rwlock_wrlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return -1;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto err;
	}

	cdlog_default_category = cdlog_category_table_fetch_category(
				cdlog_env_categories,
				cname,
				cdlog_env_conf->rules);
	if (!cdlog_default_category) {
		zc_error("cdlog_category_table_fetch_category[%s] fail", cname);
		goto err;
	}

	zc_debug("------dcdlog_set_category[%s] end, success------ ", cname);
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return 0;
err:
	zc_error("------dcdlog_set_category[%s] end, fail------ ", cname);
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return -1;
}
/*******************************************************************************/
#define cdlog_fetch_thread(a_thread, fail_goto) do {  \
	int rd = 0;  \
	a_thread = pthread_getspecific(cdlog_thread_key);  \
	if (!a_thread) {  \
		a_thread = cdlog_thread_new(cdlog_env_init_version,  \
				cdlog_env_conf->buf_size_min, cdlog_env_conf->buf_size_max, \
				cdlog_env_conf->time_cache_count); \
		if (!a_thread) {  \
			zc_error("cdlog_thread_new fail");  \
			goto fail_goto;  \
		}  \
  \
		rd = pthread_setspecific(cdlog_thread_key, a_thread);  \
		if (rd) {  \
			cdlog_thread_del(a_thread);  \
			zc_error("pthread_setspecific fail, rd[%d]", rd);  \
			goto fail_goto;  \
		}  \
	}  \
  \
	if (a_thread->init_version != cdlog_env_init_version) {  \
		/* as mdc is still here, so can not easily del and new */ \
		rd = cdlog_thread_rebuild_msg_buf(a_thread, \
				cdlog_env_conf->buf_size_min, \
				cdlog_env_conf->buf_size_max);  \
		if (rd) {  \
			zc_error("cdlog_thread_resize_msg_buf fail, rd[%d]", rd);  \
			goto fail_goto;  \
		}  \
  \
		rd = cdlog_thread_rebuild_event(a_thread, cdlog_env_conf->time_cache_count);  \
		if (rd) {  \
			zc_error("cdlog_thread_resize_msg_buf fail, rd[%d]", rd);  \
			goto fail_goto;  \
		}  \
		a_thread->init_version = cdlog_env_init_version;  \
	}  \
} while (0)

/*******************************************************************************/
int cdlog_put_mdc(const char *key, const char *value)
{
	int rc = 0;
	cdlog_thread_t *a_thread;

	zc_assert(key, -1);
	zc_assert(value, -1);

	rc = pthread_rwlock_rdlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return -1;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto err;
	}

	cdlog_fetch_thread(a_thread, err);

	if (cdlog_mdc_put(a_thread->mdc, key, value)) {
		zc_error("cdlog_mdc_put fail, key[%s], value[%s]", key, value);
		goto err;
	}

	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return 0;
err:
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return -1;
	}
	return -1;
}

char *cdlog_get_mdc(char *key)
{
	int rc = 0;
	char *value = NULL;
	cdlog_thread_t *a_thread;

	zc_assert(key, NULL);

	rc = pthread_rwlock_rdlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_rdlock fail, rc[%d]", rc);
		return NULL;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto err;
	}

	a_thread = pthread_getspecific(cdlog_thread_key);
	if (!a_thread) {
		zc_error("thread not found, maybe not use cdlog_put_mdc before");
		goto err;
	}

	value = cdlog_mdc_get(a_thread->mdc, key);
	if (!value) {
		zc_error("key[%s] not found in mdc", key);
		goto err;
	}

	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return NULL;
	}
	return value;
err:
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return NULL;
	}
	return NULL;
}

void cdlog_remove_mdc(char *key)
{
	int rc = 0;
	cdlog_thread_t *a_thread;

	zc_assert(key, );

	rc = pthread_rwlock_rdlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_rdlock fail, rc[%d]", rc);
		return;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	a_thread = pthread_getspecific(cdlog_thread_key);
	if (!a_thread) {
		zc_error("thread not found, maybe not use cdlog_put_mdc before");
		goto exit;
	}

	cdlog_mdc_remove(a_thread->mdc, key);

exit:
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return;
	}
	return;
}

void cdlog_clean_mdc(void)
{
	int rc = 0;
	cdlog_thread_t *a_thread;

	rc = pthread_rwlock_rdlock(&cdlog_env_lock);
	if (rc) {;
		zc_error("pthread_rwlock_rdlock fail, rc[%d]", rc);
		return;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	a_thread = pthread_getspecific(cdlog_thread_key);
	if (!a_thread) {
		zc_error("thread not found, maybe not use cdlog_put_mdc before");
		goto exit;
	}

	cdlog_mdc_clean(a_thread->mdc);

exit:
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return;
	}
	return;
}

int cdlog_level_switch(cdlog_category_t * category, int level)
{
    // This is NOT thread safe.
    memset(category->level_bitmap, 0x00, sizeof(category->level_bitmap));
    category->level_bitmap[level / 8] |= ~(0xFF << (8 - level % 8));
    memset(category->level_bitmap + level / 8 + 1, 0xFF,
	    sizeof(category->level_bitmap) -  level / 8 - 1);

    return 0;
}

/*******************************************************************************/
void vcdlog(cdlog_category_t * category,
	const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const char *format, va_list args)
{
	cdlog_thread_t *a_thread;

	/* The bitmap determination here is not under the protection of rdlock.
	 * It may be changed by other CPU by cdlog_reload() halfway.
	 *
	 * Old or strange value may be read here,
	 * but it is safe, the bitmap is valid as long as category exist,
	 * And will be the right value after cdlog_reload()
	 *
	 * For speed up, if one log will not be ouput,
	 * There is no need to aquire rdlock.
	 */
	if (cdlog_category_needless_level(category, level)) return;

	pthread_rwlock_rdlock(&cdlog_env_lock);

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	cdlog_fetch_thread(a_thread, exit);

	cdlog_event_set_fmt(a_thread->event,
		category->name, category->name_len,
		file, filelen, func, funclen, line, level,
		format, args);

	if (cdlog_category_output(category, a_thread)) {
		zc_error("cdlog_output fail, srcfile[%s], srcline[%ld]", file, line);
		goto exit;
	}

	if (cdlog_env_conf->reload_conf_period &&
		++cdlog_env_reload_conf_count > cdlog_env_conf->reload_conf_period ) {
		/* under the protection of lock read env conf */
		goto reload;
	}

exit:
	pthread_rwlock_unlock(&cdlog_env_lock);
	return;
reload:
	pthread_rwlock_unlock(&cdlog_env_lock);
	/* will be wrlock, so after unlock */
	if (cdlog_reload((char *)-1)) {
		zc_error("reach reload-conf-period but cdlog_reload fail, cdlog-chk-conf [file] see detail");
	}
	return;
}

void hcdlog(cdlog_category_t *category,
	const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const void *buf, size_t buflen)
{
	cdlog_thread_t *a_thread;

	if (cdlog_category_needless_level(category, level)) return;

	pthread_rwlock_rdlock(&cdlog_env_lock);

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	cdlog_fetch_thread(a_thread, exit);

	cdlog_event_set_hex(a_thread->event,
		category->name, category->name_len,
		file, filelen, func, funclen, line, level,
		buf, buflen);

	if (cdlog_category_output(category, a_thread)) {
		zc_error("cdlog_output fail, srcfile[%s], srcline[%ld]", file, line);
		goto exit;
	}

	if (cdlog_env_conf->reload_conf_period &&
		++cdlog_env_reload_conf_count > cdlog_env_conf->reload_conf_period ) {
		/* under the protection of lock read env conf */
		goto reload;
	}

exit:
	pthread_rwlock_unlock(&cdlog_env_lock);
	return;
reload:
	pthread_rwlock_unlock(&cdlog_env_lock);
	/* will be wrlock, so after unlock */
	if (cdlog_reload((char *)-1)) {
		zc_error("reach reload-conf-period but cdlog_reload fail, cdlog-chk-conf [file] see detail");
	}
	return;
}

/*******************************************************************************/
/* for speed up, copy from vcdlog */
void vdcdlog(const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const char *format, va_list args)
{
	cdlog_thread_t *a_thread;

	if (cdlog_category_needless_level(cdlog_default_category, level)) return;

	pthread_rwlock_rdlock(&cdlog_env_lock);

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	/* that's the differnce, must judge default_category in lock */
	if (!cdlog_default_category) {
		zc_error("cdlog_default_category is null,"
			"dcdlog_init() or dcdlog_set_cateogry() is not called above");
		goto exit;
	}

	cdlog_fetch_thread(a_thread, exit);

	cdlog_event_set_fmt(a_thread->event,
		cdlog_default_category->name, cdlog_default_category->name_len,
		file, filelen, func, funclen, line, level,
		format, args);

	if (cdlog_category_output(cdlog_default_category, a_thread)) {
		zc_error("cdlog_output fail, srcfile[%s], srcline[%ld]", file, line);
		goto exit;
	}

	if (cdlog_env_conf->reload_conf_period &&
		++cdlog_env_reload_conf_count > cdlog_env_conf->reload_conf_period ) {
		/* under the protection of lock read env conf */
		goto reload;
	}

exit:
	pthread_rwlock_unlock(&cdlog_env_lock);
	return;
reload:
	pthread_rwlock_unlock(&cdlog_env_lock);
	/* will be wrlock, so after unlock */
	if (cdlog_reload((char *)-1)) {
		zc_error("reach reload-conf-period but cdlog_reload fail, cdlog-chk-conf [file] see detail");
	}
	return;
}

void hdcdlog(const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const void *buf, size_t buflen)
{
	cdlog_thread_t *a_thread;

	if (cdlog_category_needless_level(cdlog_default_category, level)) return;

	pthread_rwlock_rdlock(&cdlog_env_lock);

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	/* that's the differnce, must judge default_category in lock */
	if (!cdlog_default_category) {
		zc_error("cdlog_default_category is null,"
			"dcdlog_init() or dcdlog_set_cateogry() is not called above");
		goto exit;
	}

	cdlog_fetch_thread(a_thread, exit);

	cdlog_event_set_hex(a_thread->event,
		cdlog_default_category->name, cdlog_default_category->name_len,
		file, filelen, func, funclen, line, level,
		buf, buflen);

	if (cdlog_category_output(cdlog_default_category, a_thread)) {
		zc_error("cdlog_output fail, srcfile[%s], srcline[%ld]", file, line);
		goto exit;
	}

	if (cdlog_env_conf->reload_conf_period &&
		++cdlog_env_reload_conf_count > cdlog_env_conf->reload_conf_period ) {
		/* under the protection of lock read env conf */
		goto reload;
	}

exit:
	pthread_rwlock_unlock(&cdlog_env_lock);
	return;
reload:
	pthread_rwlock_unlock(&cdlog_env_lock);
	/* will be wrlock, so after unlock */
	if (cdlog_reload((char *)-1)) {
		zc_error("reach reload-conf-period but cdlog_reload fail, cdlog-chk-conf [file] see detail");
	}
	return;
}

/*******************************************************************************/
void cdlog(cdlog_category_t * category,
	const char *file, size_t filelen, const char *func, size_t funclen,
	long line, const int level,
	const char *format, ...)
{
	cdlog_thread_t *a_thread;
	va_list args;

	if (category && cdlog_category_needless_level(category, level)) return;

	pthread_rwlock_rdlock(&cdlog_env_lock);

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	cdlog_fetch_thread(a_thread, exit);

	va_start(args, format);
	cdlog_event_set_fmt(a_thread->event, category->name, category->name_len,
		file, filelen, func, funclen, line, level,
		format, args);
	if (cdlog_category_output(category, a_thread)) {
		zc_error("cdlog_output fail, srcfile[%s], srcline[%ld]", file, line);
		va_end(args);
		goto exit;
	}
	va_end(args);

	if (cdlog_env_conf->reload_conf_period &&
		++cdlog_env_reload_conf_count > cdlog_env_conf->reload_conf_period ) {
		/* under the protection of lock read env conf */
		goto reload;
	}

exit:
	pthread_rwlock_unlock(&cdlog_env_lock);
	return;
reload:
	pthread_rwlock_unlock(&cdlog_env_lock);
	/* will be wrlock, so after unlock */
	if (cdlog_reload((char *)-1)) {
		zc_error("reach reload-conf-period but cdlog_reload fail, cdlog-chk-conf [file] see detail");
	}
	return;
}

/*******************************************************************************/
void dcdlog(const char *file, size_t filelen, const char *func, size_t funclen, long line, int level,
	const char *format, ...)
{
	cdlog_thread_t *a_thread;
	va_list args;


	pthread_rwlock_rdlock(&cdlog_env_lock);

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto exit;
	}

	/* that's the differnce, must judge default_category in lock */
	if (!cdlog_default_category) {
		zc_error("cdlog_default_category is null,"
			"dcdlog_init() or dcdlog_set_cateogry() is not called above");
		goto exit;
	}

	if (cdlog_category_needless_level(cdlog_default_category, level)) goto exit;

	cdlog_fetch_thread(a_thread, exit);

	va_start(args, format);
	cdlog_event_set_fmt(a_thread->event,
		cdlog_default_category->name, cdlog_default_category->name_len,
		file, filelen, func, funclen, line, level,
		format, args);

	if (cdlog_category_output(cdlog_default_category, a_thread)) {
		zc_error("cdlog_output fail, srcfile[%s], srcline[%ld]", file, line);
		va_end(args);
		goto exit;
	}
	va_end(args);

	if (cdlog_env_conf->reload_conf_period &&
		++cdlog_env_reload_conf_count > cdlog_env_conf->reload_conf_period ) {
		/* under the protection of lock read env conf */
		goto reload;
	}

exit:
	pthread_rwlock_unlock(&cdlog_env_lock);
	return;
reload:
	pthread_rwlock_unlock(&cdlog_env_lock);
	/* will be wrlock, so after unlock */
	if (cdlog_reload((char *)-1)) {
		zc_error("reach reload-conf-period but cdlog_reload fail, cdlog-chk-conf [file] see detail");
	}
	return;
}

/*******************************************************************************/
void cdlog_profile(void)
{
	int rc = 0;
	rc = pthread_rwlock_rdlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_wrlock fail, rc[%d]", rc);
		return;
	}
	zc_warn("------cdlog_profile start------ ");
	zc_warn("is init:[%d]", cdlog_env_is_init);
	zc_warn("init version:[%d]", cdlog_env_init_version);
	cdlog_conf_profile(cdlog_env_conf, ZC_WARN);
	cdlog_record_table_profile(cdlog_env_records, ZC_WARN);
	cdlog_category_table_profile(cdlog_env_categories, ZC_WARN);
	if (cdlog_default_category) {
		zc_warn("-default_category-");
		cdlog_category_profile(cdlog_default_category, ZC_WARN);
	}
	zc_warn("------cdlog_profile end------ ");
	rc = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rc) {
		zc_error("pthread_rwlock_unlock fail, rc=[%d]", rc);
		return;
	}
	return;
}
/*******************************************************************************/
int cdlog_set_record(const char *rname, cdlog_record_fn record_output)
{
	int rc = 0;
	int rd = 0;
	cdlog_rule_t *a_rule;
	cdlog_record_t *a_record;
	int i = 0;

	zc_assert(rname, -1);
	zc_assert(record_output, -1);

	rd = pthread_rwlock_wrlock(&cdlog_env_lock);
	if (rd) {
		zc_error("pthread_rwlock_rdlock fail, rd[%d]", rd);
		return -1;
	}

	if (!cdlog_env_is_init) {
		zc_error("never call cdlog_init() or dcdlog_init() before");
		goto cdlog_set_record_exit;
	}

	a_record = cdlog_record_new(rname, record_output);
	if (!a_record) {
		rc = -1;
		zc_error("cdlog_record_new fail");
		goto cdlog_set_record_exit;
	}

	rc = zc_hashtable_put(cdlog_env_records, a_record->name, a_record);
	if (rc) {
		cdlog_record_del(a_record);
		zc_error("zc_hashtable_put fail");
		goto cdlog_set_record_exit;
	}

	zc_arraylist_foreach(cdlog_env_conf->rules, i, a_rule) {
		cdlog_rule_set_record(a_rule, cdlog_env_records);
	}

      cdlog_set_record_exit:
	rd = pthread_rwlock_unlock(&cdlog_env_lock);
	if (rd) {
		zc_error("pthread_rwlock_unlock fail, rd=[%d]", rd);
		return -1;
	}
	return rc;
}
/*******************************************************************************/
int cdlog_level_enabled(cdlog_category_t *category, const int level)
{
	return category && (cdlog_category_needless_level(category, level) == 0);
}

const char *cdlog_version(void) { return ZLOG_VERSION; }
