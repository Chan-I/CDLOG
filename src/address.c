#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "zc_defs.h"
#include "thread.h"
#include "spec.h"
#include "address.h"

void
cdlog_address_profile(cdlog_address_t *a_address, int flag)
{
    zc_assert(a_address,);
    zc_profile(flag, "---address[%p][%s = %s(%p)]---",
            a_address,
            a_address->type,
            a_address->url,
            a_address->url_specs);
    return;
}

void
cdlog_address_del(cdlog_address_t *a_address)
{
    zc_assert(a_address, );
    if (a_address->url_specs) {
        zc_arraylist_del(a_address->url_specs);
    }
    zc_debug("zc_address_list_del[%p]", a_address);
    free(a_address);
    return;
}

cdlog_address_t * 
cdlog_address_new(const char *line, int * time_cache_count)
{
    int nscan = 0, nread = 0;
    cdlog_address_t *addresses = NULL;
    char msg[MAXLEN_CFG_LINE + 1] = {0};
    char *p, *q;
    const char *p_start;
    const char *p_end;
    cdlog_spec_t *a_spec;

    zc_assert(line, NULL);

    addresses = calloc(1, sizeof(cdlog_address_t));
    if (!addresses){
        zc_error("calloc fail, errno [%d]", errno);
        return NULL;
    }

    /*  line : restful url = "http://localhost:8080/data/pages=1.idex"
     *  type : restful
     *  url  : http://localhost:8080/data/pages=1.idex
     */
    memset(addresses->type, 0x00, sizeof(addresses->type));
    nscan = sscanf(line, "%[^=] = %n", msg, &nread);
    if (nscan != 1){
        zc_error("sscanf [%s] fail, method or address's url is null", line);
        goto err;
    }

    nscan = sscanf(msg, "%[^= \t]", addresses->type);
    if (*(line + nread) != '"') {
        zc_error("the first char of url is not \", line+nread[%s]", line + nread);
        goto err;
    }

    if (IEQUAL(addresses->type, "restful")
        && IEQUAL(addresses->type, "http")
        && IEQUAL(addresses->type, "https")) {
        zc_error("the first keyword of addresses->type is not [restful].");
        goto err;
    }

    p_start = line + nread + 1;
    p_end = strrchr(p_start, '"');
    if (!p_end) {
        zc_error("there is no \" at the end of url address, line[%s]", line);
        goto err;
    }

    if (p_end - p_start > sizeof(addresses->url) - 1) {
        zc_error("url is too long.");
        goto err;
    }

    memset(addresses->url, 0x00, sizeof(addresses->url));
    memcpy(addresses->url, p_start, p_end - p_start);

    if (zc_str_replace_env(addresses->url, sizeof(addresses->url))) {
        zc_error("zc_str_replace_env failed.");
        goto err;
    }
    addresses->url_specs = zc_arraylist_new((zc_arraylist_del_fn)cdlog_spec_del);
    if (!(addresses->url_specs)) {
        zc_error("zc_arraylist_new fail.");
        goto err;
    }

    for (p = addresses->url; *p != '\0'; p = q) {
        a_spec = cdlog_spec_new(p, &q, time_cache_count);
        if (!a_spec) {
            zc_error("cdlog_spec_new fail");
            goto err;
        }

        if (zc_arraylist_add(addresses->url_specs, a_spec)) {
            cdlog_spec_del(a_spec);
            zc_error("zc_arraylist_add fial");
            goto err;
        }
    }
    
    cdlog_address_profile(addresses, ZC_DEBUG);
    return addresses;

err:
    cdlog_address_del(addresses);
    return NULL;
}

