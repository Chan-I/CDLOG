/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <pthread.h>
#include <errno.h>

#include "zc_defs.h"
#include "event.h"
#include "buf.h"
#include "thread.h"
#include "mdc.h"

void cdlog_thread_profile(cdlog_thread_t * a_thread, int flag)
{
	zc_assert(a_thread,);
	zc_profile(flag, "--thread[%p][%p][%p][%p,%p,%p,%p,%p]--",
			a_thread,
			a_thread->mdc,
			a_thread->event,
			a_thread->pre_path_buf,
			a_thread->path_buf,
			a_thread->archive_path_buf,
			a_thread->pre_msg_buf,
			a_thread->msg_buf);

	cdlog_mdc_profile(a_thread->mdc, flag);
	cdlog_event_profile(a_thread->event, flag);
	cdlog_buf_profile(a_thread->pre_path_buf, flag);
	cdlog_buf_profile(a_thread->path_buf, flag);
	cdlog_buf_profile(a_thread->archive_path_buf, flag);
	cdlog_buf_profile(a_thread->pre_msg_buf, flag);
	cdlog_buf_profile(a_thread->msg_buf, flag);
	return;
}
/*******************************************************************************/
void cdlog_thread_del(cdlog_thread_t * a_thread)
{
	zc_assert(a_thread,);
	if (a_thread->mdc)
		cdlog_mdc_del(a_thread->mdc);
	if (a_thread->event)
		cdlog_event_del(a_thread->event);
	if (a_thread->pre_path_buf)
		cdlog_buf_del(a_thread->pre_path_buf);
	if (a_thread->path_buf)
		cdlog_buf_del(a_thread->path_buf);
	if (a_thread->archive_path_buf)
		cdlog_buf_del(a_thread->archive_path_buf);
	if (a_thread->pre_msg_buf)
		cdlog_buf_del(a_thread->pre_msg_buf);
	if (a_thread->msg_buf)
		cdlog_buf_del(a_thread->msg_buf);

	zc_debug("cdlog_thread_del[%p]", a_thread);
    free(a_thread);
	return;
}

cdlog_thread_t *cdlog_thread_new(int init_version, size_t buf_size_min, size_t buf_size_max, int time_cache_count)
{
	cdlog_thread_t *a_thread;

	a_thread = calloc(1, sizeof(cdlog_thread_t));
	if (!a_thread) {
		zc_error("calloc fail, errno[%d]", errno);
		return NULL;
	}

	a_thread->init_version = init_version;

	a_thread->mdc = cdlog_mdc_new();
	if (!a_thread->mdc) {
		zc_error("cdlog_mdc_new fail");
		goto err;
	}

	a_thread->event = cdlog_event_new(time_cache_count);
	if (!a_thread->event) {
		zc_error("cdlog_event_new fail");
		goto err;
	}

	a_thread->pre_path_buf = cdlog_buf_new(MAXLEN_PATH + 1, MAXLEN_PATH + 1, NULL);
	if (!a_thread->pre_path_buf) {
		zc_error("cdlog_buf_new fail");
		goto err;
	}

	a_thread->path_buf = cdlog_buf_new(MAXLEN_PATH + 1, MAXLEN_PATH + 1, NULL);
	if (!a_thread->path_buf) {
		zc_error("cdlog_buf_new fail");
		goto err;
	}

	a_thread->archive_path_buf = cdlog_buf_new(MAXLEN_PATH + 1, MAXLEN_PATH + 1, NULL);
	if (!a_thread->archive_path_buf) {
		zc_error("cdlog_buf_new fail");
		goto err;
	}

	a_thread->pre_msg_buf = cdlog_buf_new(buf_size_min, buf_size_max, "..." FILE_NEWLINE);
	if (!a_thread->pre_msg_buf) {
		zc_error("cdlog_buf_new fail");
		goto err;
	}

	a_thread->msg_buf = cdlog_buf_new(buf_size_min, buf_size_max, "..." FILE_NEWLINE);
	if (!a_thread->msg_buf) {
		zc_error("cdlog_buf_new fail");
		goto err;
	}


	//cdlog_thread_profile(a_thread, ZC_DEBUG);
	return a_thread;
err:
	cdlog_thread_del(a_thread);
	return NULL;
}

/*******************************************************************************/
int cdlog_thread_rebuild_msg_buf(cdlog_thread_t * a_thread, size_t buf_size_min, size_t buf_size_max)
{
	cdlog_buf_t *pre_msg_buf_new = NULL;
	cdlog_buf_t *msg_buf_new = NULL;
	zc_assert(a_thread, -1);

	if ( (a_thread->msg_buf->size_min == buf_size_min)
		&& (a_thread->msg_buf->size_max == buf_size_max)) {
		zc_debug("buf size not changed, no need rebuild");
		return 0;
	}

	pre_msg_buf_new = cdlog_buf_new(buf_size_min, buf_size_max, "..." FILE_NEWLINE);
	if (!pre_msg_buf_new) {
		zc_error("cdlog_buf_new fail");
		goto err;
	}

	msg_buf_new = cdlog_buf_new(buf_size_min, buf_size_max, "..." FILE_NEWLINE);
	if (!msg_buf_new) {
		zc_error("cdlog_buf_new fail");
		goto err;
	}

	cdlog_buf_del(a_thread->pre_msg_buf);
	a_thread->pre_msg_buf = pre_msg_buf_new;

	cdlog_buf_del(a_thread->msg_buf);
	a_thread->msg_buf = msg_buf_new;

	return 0;
err:
	if (pre_msg_buf_new) cdlog_buf_del(pre_msg_buf_new);
	if (msg_buf_new) cdlog_buf_del(msg_buf_new);
	return -1;
}

int cdlog_thread_rebuild_event(cdlog_thread_t * a_thread, int time_cache_count)
{
	cdlog_event_t *event_new = NULL;
	zc_assert(a_thread, -1);

	event_new = cdlog_event_new(time_cache_count);
	if (!event_new) {
		zc_error("cdlog_event_new fail");
		goto err;
	}

	cdlog_event_del(a_thread->event);
	a_thread->event = event_new;
	return 0;
err:
	if (event_new) cdlog_event_del(event_new);
	return -1;
}


/*******************************************************************************/
