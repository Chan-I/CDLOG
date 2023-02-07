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

	rc = cdlog_init("test_mdc.conf");
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


	cdlog_info(zc, "1.hello, cdlog");

	cdlog_put_mdc("myname", "Zhang");

	cdlog_info(zc, "2.hello, cdlog");

	cdlog_put_mdc("myname", "Li");

	cdlog_info(zc, "3.hello, cdlog");

	cdlog_fini();
	
	return 0;
}
