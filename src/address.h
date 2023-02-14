#ifndef __cdlog_address_h
#define __cdlog_address_h

#include "thread.h"
#include "zc_defs.h"

#define IEQUAL(_a, _b) (STRICMP(_a, ==, _b) != 0)

typedef struct cdlog_address_s cdlog_address_t;

struct cdlog_address_s
{
	/* 
	 * 	[address]
	 * 	restful url="http://localhost:8080/data/get.index" 
	 */
	char type[MAXLEN_CFG_LINE + 1];	/* restful */
	char url[MAXLEN_CFG_LINE + 1];	/* "http://localhost:8080/data/get.index" */
	zc_arraylist_t *url_specs;
};

void cdlog_address_profile(cdlog_address_t *a_address, int flag);
void cdlog_address_del(cdlog_address_t *a_address);
cdlog_address_t * cdlog_address_new(const char * line, int * time_cache_count);

#endif
