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
	int k;
	int i;

	if (argc != 2) {
		printf("test_leak ntime\n");
		return -1;
	}

	rc = cdlog_init("test_leak.conf");

	k = atoi(argv[1]);
	while (k-- > 0) {
		i = rand();
		switch (i % 4) {
		case 0:
			rc = dcdlog_init("test_leak.conf", "xxx");
			dcdlog_info("init, rc=[%d]", rc);
			break;
		case 1:
			rc = cdlog_reload(NULL);
			dcdlog_info("reload null, rc=[%d]", rc);
			break;
		case 2:
			rc = cdlog_reload("test_leak.2.conf");
			dcdlog_info("reload 2, rc=[%d]", rc);
			break;
		case 3:
			cdlog_fini();
			printf("fini\n");
	//		printf("cdlog_finish\tj=[%d], rc=[%d]\n", j, rc);
			break;
		}
	}

	cdlog_fini();
	return 0;
}
