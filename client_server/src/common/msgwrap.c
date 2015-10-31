/**
 * @file msgwrap.c
 * @brief length check is required for memcpy in this file.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Sat 2015-10-31 19:18:29 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <pthread.h>
#include  <semaphore.h>
#include  <netinet/in.h>
#endif

#include  <stdio.h>
#include  <assert.h>
#include  <string.h>
#include  <stdint.h>
#include    "macros.h"
#include    "utility_wrap.h"
#include	"sock_types.h"
#include    "lightthread.h"
#include    "bufarray.h"
#include    "sendrecv_pool.h"
#include    "msgwrap.h"


int csmsg_merge(const struct csmsg_header* msgheader, const char* data, char* unit, int unitlen)
{
    int len_unitheader = sizeof(struct csmsg_header);

    assert((len_unitheader + msgheader->numbytes) < unitlen);
    if (cs_memcpy(unit, unitlen, msgheader, len_unitheader) != 0) {
		return 1;
	}
	if (cs_memcpy(unit + len_unitheader, unitlen - len_unitheader, data, msgheader->numbytes) != 0) {
		return 1;
	}

    return 0;
}

void csmsg_extract(const char* unit, const struct csmsg_header** msgheader, const char** data)
{
    *msgheader = (const struct csmsg_header*)unit;
    *data = (const char*)(unit + sizeof(struct csmsg_header));
}

int csmsg_extract_copy(const char* unit, struct csmsg_header* msgheader, char* data, int datalen)
{
    int len_unitheader = sizeof(struct csmsg_header);

    if (cs_memcpy(msgheader, len_unitheader, unit, len_unitheader) != 0) {
        return 1;
    }
    
    if (cs_memcpy(data, datalen, unit + len_unitheader, msgheader->numbytes) != 0) {
        return 1;
    }
    
    return 0;
}

int csmsg_push2pool(const char* data, const struct csmsg_header* msgheader, struct cssendrecv_pool* pool)
{
    char* bufitem = cspool_pullitem(pool, &pool->empty_buf);

    if (bufitem == NULL) {
        return -1;
    }

    if (csmsg_merge(msgheader, data, bufitem, pool->len_item) != 0) {
        printf("copy data to pool error, omit current data.\n");
        cspool_pushitem(pool, &pool->empty_buf, bufitem);
        return 1;
    }

    cspool_pushitem(pool, &pool->filled_buf, bufitem);
    return 0;
}

int csmsg_pull_from_pool(char* data, int datalen, struct csmsg_header* msgheader, struct cssendrecv_pool* pool)
{
    char* bufitem = cspool_pullitem(pool, &pool->filled_buf);

    if (bufitem == NULL) {
        return -1;
    }

    if (csmsg_extract_copy(bufitem, msgheader, data, datalen) != 0) {
        fprintf(stderr, "copy data from pool error, cancel.\n");
        cspool_pushitem(pool, &pool->filled_buf, bufitem);
        return 1;
    }

    cspool_pushitem(pool, &pool->empty_buf, bufitem);
    return 0;
}
