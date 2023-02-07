/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "cdlog.h"

int main(int argc, char** argv)
{
	int rc;
	
	cdlog_category_t *zc;

	rc = cdlog_init("test_init.conf");
	if (rc) {
		printf("init fail");
		return -2;
	}
	zc = cdlog_get_category("my_cat");
	if (!zc) {
		printf("cdlog_get_category fail\n");
		cdlog_fini();
		return -1;
	}
	cdlog_info(zc, "before update");
	sleep(1);
	rc = cdlog_reload("test_init.2.conf");
	if (rc) {
		printf("update fail\n");
	}
	cdlog_info(zc, "after update");
	cdlog_profile();
	cdlog_fini();

	sleep(1);
	cdlog_init("test_init.conf");
	zc = cdlog_get_category("my_cat");
	if (!zc) {
		printf("cdlog_get_category fail\n");
		cdlog_fini();
		return -1;
	}
	cdlog_info(zc, "init again");
	cdlog_fini();
	
	return 0;
}
