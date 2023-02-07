/*
 * This file is part of the cdlog Library.
 *
 * Copyright (C) 2011 by Hardy Simpson <HardySimpson1984@gmail.com>
 *
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 */

#include <stdio.h>
#include "zc_defs.h"
#include "buf.h"

int main(int argc, char** argv)
{
	cdlog_buf_t *a_buf;
	char *aa;

	a_buf = cdlog_buf_new(10, 20, "ABC");
	if (!a_buf) {
		zc_error("cdlog_buf_new fail");
		return -1;
	}

	aa = "123456789";
	cdlog_buf_append(a_buf, aa, strlen(aa));
	zc_error("a_buf->start[%s]", a_buf->start);
	fwrite(a_buf->start, cdlog_buf_len(a_buf), 1, stdout);
	zc_error("------------");

	aa = "0";
	cdlog_buf_append(a_buf, aa, strlen(aa));
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	aa = "12345";
	cdlog_buf_append(a_buf, aa, strlen(aa));
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	aa = "6789";
	cdlog_buf_append(a_buf, aa, strlen(aa));
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	aa = "0";
	cdlog_buf_append(a_buf, aa, strlen(aa));
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	aa = "22345";
	cdlog_buf_append(a_buf, aa, strlen(aa));
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");


	aa = "abc";
	int i,j;
	for (i = 0; i <= 5; i++) {
		for (j = 0; j <= 5; j++) {
			cdlog_buf_restart(a_buf);
			zc_error("left[1],max[%d],min[%d]", i, j);
			cdlog_buf_adjust_append(a_buf, aa, strlen(aa), 1, 0, i, j);
			zc_error("a_buf->start[%s]", a_buf->start);

			zc_error("-----");

			cdlog_buf_restart(a_buf);
			zc_error("left[0],max[%d],min[%d]", i, j);
			cdlog_buf_adjust_append(a_buf, aa, strlen(aa), 0, 0, i, j);
			zc_error("a_buf->start[%s]", a_buf->start);
			zc_error("------------");
		}
	}

	aa = "1234567890";
	zc_error("left[0],max[%d],min[%d]", 15, 5);
	cdlog_buf_adjust_append(a_buf, aa, strlen(aa), 0, 0, 15, 5);
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	aa = "1234567890";
	cdlog_buf_restart(a_buf);
	zc_error("left[0],max[%d],min[%d]", 25, 5);
	cdlog_buf_adjust_append(a_buf, aa, strlen(aa), 1, 0, 25, 5);
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	cdlog_buf_restart(a_buf);
	zc_error("left[0],max[%d],min[%d]", 19, 5);
	cdlog_buf_adjust_append(a_buf, aa, strlen(aa), 0, 0, 19, 5);
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	cdlog_buf_restart(a_buf);
	zc_error("left[0],max[%d],min[%d]", 20, 5);
	cdlog_buf_adjust_append(a_buf, aa, strlen(aa), 0, 0, 20, 5);
	zc_error("a_buf->start[%s]", a_buf->start);
	zc_error("------------");

	cdlog_buf_del(a_buf);

	return 0;
}
