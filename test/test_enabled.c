/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2018 by Teracom Telemática S/A
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdio.h>
#include "test_enabled.h"

int main(int argc, char** argv)
{
	int rc;
	cdlog_category_t *zc;

	rc = cdlog_init("test_enabled.conf");
	if (rc) {
		printf("init failed\n");
		return -1;
	}

	zc = cdlog_get_category("my_cat");
	if (!zc) {
		printf("get cat fail\n");
		cdlog_fini();
		return -2;
	}

	if (cdlog_trace_enabled(zc)) {
		/* do something heavy to collect data */
		cdlog_trace(zc, "hello, cdlog - trace");
	}

	if (cdlog_debug_enabled(zc)) {
		/* do something heavy to collect data */
		cdlog_debug(zc, "hello, cdlog - debug");
	}

	if (cdlog_info_enabled(zc)) {
		/* do something heavy to collect data */
		cdlog_info(zc, "hello, cdlog - info");
	}

	cdlog_fini();

	return 0;
}
