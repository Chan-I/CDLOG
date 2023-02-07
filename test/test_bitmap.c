/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cdlog.h"

int main(int argc, char** argv)
{
	unsigned char aa[32];
	int i, j;

	if (argc != 3) {
		printf("useage: test_bitmap i j\n");
		exit(1);
	}

	dcdlog_init(NULL, "AA");


	i = atoi(argv[1]);
	j = atoi(argv[2]);

	memset(aa, 0x00, sizeof(aa));

	/* 32 byte, 256 bit
	 * [11111..1100...00]
	 *          i
	 */
	aa[i/8] |=  ~(0xFF << (8 - i % 8));
	memset(aa + i/8 + 1, 0xFF, sizeof(aa) - i/8 - 1);

	hdcdlog_info(aa, sizeof(aa));

	dcdlog_info("%0x", aa[j/8]);
	dcdlog_info("%0x", aa[j/8] >> 6);

	/* see j of bits fits */
	dcdlog_info("%0x", ~((aa[j/8] >> (7 - j % 8)) & 0x01) );

	cdlog_fini();
	
	return 0;
}
