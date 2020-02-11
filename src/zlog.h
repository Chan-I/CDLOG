/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#ifndef __zlog_h
#define __zlog_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h> /* for va_list */
#include <stdio.h> /* for size_t */

# if defined __GNUC__
#   define ZLOG_CHECK_PRINTF(m,n) __attribute__((format(printf,m,n)))
# else 
#   define ZLOG_CHECK_PRINTF(m,n)
# endif

typedef struct cdlog_category_s cdlog_category_t;

int cdlog_init(const char *confpath);
int cdlog_reload(const char *confpath);
void cdlog_fini(void);

void cdlog_profile(void);

cdlog_category_t *cdlog_get_category(const char *cname);
int cdlog_level_enabled(cdlog_category_t *category, const int level);

int cdlog_put_mdc(const char *key, const char *value);
char *cdlog_get_mdc(const char *key);
void cdlog_remove_mdc(const char *key);
void cdlog_clean_mdc(void);

int cdlog_level_switch(cdlog_category_t * category, int level);
int cdlog_level_enabled(cdlog_category_t * category, int level);

void cdlog(cdlog_category_t * category,
	const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const char *format, ...) ZLOG_CHECK_PRINTF(8,9);
void vcdlog(cdlog_category_t * category,
	const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const char *format, va_list args);
void hcdlog(cdlog_category_t * category,
	const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const void *buf, size_t buflen);

int dcdlog_init(const char *confpath, const char *cname);
int dcdlog_set_category(const char *cname);

void dcdlog(const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const char *format, ...) ZLOG_CHECK_PRINTF(7,8);
void vdcdlog(const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const char *format, va_list args);
void hdcdlog(const char *file, size_t filelen,
	const char *func, size_t funclen,
	long line, int level,
	const void *buf, size_t buflen);

typedef struct cdlog_msg_s {
	char *buf;
	size_t len;
	char *path;
} cdlog_msg_t;

typedef int (*cdlog_record_fn)(cdlog_msg_t *msg);
int cdlog_set_record(const char *rname, cdlog_record_fn record);

const char *cdlog_version(void);

/******* useful macros, can be redefined at user's h file **********/

typedef enum {
	ZLOG_LEVEL_DEBUG = 20,
	ZLOG_LEVEL_INFO = 40,
	ZLOG_LEVEL_NOTICE = 60,
	ZLOG_LEVEL_WARN = 80,
	ZLOG_LEVEL_ERROR = 100,
	ZLOG_LEVEL_FATAL = 120
} cdlog_level; 

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
# if defined __GNUC__ && __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
/* cdlog macros */
#define cdlog_fatal(cat, ...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, __VA_ARGS__)
#define cdlog_error(cat, ...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, __VA_ARGS__)
#define cdlog_warn(cat, ...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, __VA_ARGS__)
#define cdlog_notice(cat, ...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, __VA_ARGS__)
#define cdlog_info(cat, ...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, __VA_ARGS__)
#define cdlog_debug(cat, ...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, __VA_ARGS__)
/* dcdlog macros */
#define dcdlog_fatal(...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, __VA_ARGS__)
#define dcdlog_error(...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, __VA_ARGS__)
#define dcdlog_warn(...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, __VA_ARGS__)
#define dcdlog_notice(...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, __VA_ARGS__)
#define dcdlog_info(...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, __VA_ARGS__)
#define dcdlog_debug(...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, __VA_ARGS__)
#elif defined __GNUC__
/* cdlog macros */
#define cdlog_fatal(cat, format, args...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define cdlog_error(cat, format, args...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define cdlog_warn(cat, format, args...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)
#define cdlog_notice(cat, format, args...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define cdlog_info(cat, format, args...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define cdlog_debug(cat, format, args...) \
	cdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
/* dcdlog macros */
#define dcdlog_fatal(format, args...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, ##args)
#define dcdlog_error(format, args...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, ##args)
#define dcdlog_warn(format, args...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, ##args)
#define dcdlog_notice(format, args...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, ##args)
#define dcdlog_info(format, args...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, ##args)
#define dcdlog_debug(format, args...) \
	dcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, ##args)
#endif

/* vcdlog macros */
#define vcdlog_fatal(cat, format, args) \
	vcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, args)
#define vcdlog_error(cat, format, args) \
	vcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, args)
#define vcdlog_warn(cat, format, args) \
	vcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, args)
#define vcdlog_notice(cat, format, args) \
	vcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, args)
#define vcdlog_info(cat, format, args) \
	vcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, args)
#define vcdlog_debug(cat, format, args) \
	vcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, args)

/* hcdlog macros */
#define hcdlog_fatal(cat, buf, buf_len) \
	hcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, buf, buf_len)
#define hcdlog_error(cat, buf, buf_len) \
	hcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, buf, buf_len)
#define hcdlog_warn(cat, buf, buf_len) \
	hcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, buf, buf_len)
#define hcdlog_notice(cat, buf, buf_len) \
	hcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, buf, buf_len)
#define hcdlog_info(cat, buf, buf_len) \
	hcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, buf, buf_len)
#define hcdlog_debug(cat, buf, buf_len) \
	hcdlog(cat, __FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, buf, buf_len)


/* vdcdlog macros */
#define vdcdlog_fatal(format, args) \
	vdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, format, args)
#define vdcdlog_error(format, args) \
	vdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, format, args)
#define vdcdlog_warn(format, args) \
	vdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, format, args)
#define vdcdlog_notice(format, args) \
	vdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, format, args)
#define vdcdlog_info(format, args) \
	vdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, format, args)
#define vdcdlog_debug(format, args) \
	vdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, format, args)

/* hdcdlog macros */
#define hdcdlog_fatal(buf, buf_len) \
	hdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_FATAL, buf, buf_len)
#define hdcdlog_error(buf, buf_len) \
	hdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_ERROR, buf, buf_len)
#define hdcdlog_warn(buf, buf_len) \
	hdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_WARN, buf, buf_len)
#define hdcdlog_notice(buf, buf_len) \
	hdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_NOTICE, buf, buf_len)
#define hdcdlog_info(buf, buf_len) \
	hdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_INFO, buf, buf_len)
#define hdcdlog_debug(buf, buf_len) \
	hdcdlog(__FILE__, sizeof(__FILE__)-1, __func__, sizeof(__func__)-1, __LINE__, \
	ZLOG_LEVEL_DEBUG, buf, buf_len)

/* enabled macros */
#define cdlog_fatal_enabled(zc) cdlog_level_enabled(zc, ZLOG_LEVEL_FATAL)
#define cdlog_error_enabled(zc) cdlog_level_enabled(zc, ZLOG_LEVEL_ERROR)
#define cdlog_warn_enabled(zc) cdlog_level_enabled(zc, ZLOG_LEVEL_WARN)
#define cdlog_notice_enabled(zc) cdlog_level_enabled(zc, ZLOG_LEVEL_NOTICE)
#define cdlog_info_enabled(zc) cdlog_level_enabled(zc, ZLOG_LEVEL_INFO)
#define cdlog_debug_enabled(zc) cdlog_level_enabled(zc, ZLOG_LEVEL_DEBUG)

#ifdef __cplusplus
}
#endif

#endif
