/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdio.h>
#include "cdlog.h"

int main(int argc, char** argv)
{
	int rc;

	rc = dcdlog_init("test_default.conf", "my_cat");
	if (rc) {
		printf("init failed\n");
		return -1;
	}

	dcdlog_info("hello, cdlog");

	cdlog_fini();
	
	return 0;
}
