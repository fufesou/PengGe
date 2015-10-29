/**
 * @file msgwrap.c
 * @brief length check is required for memcpy in this file.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Mon 2015-10-26 18:42:49 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#endif

#include  <stdio.h>
#include  <assert.h>
#include  <string.h>
#include  <stdint.h>
#include    "macros.h"
#include	"sock_types.h"
#include    "lightthread.h"
#include    "bufarray.h"
#include    "sendrecv_pool.h"
#include    "msgwrap.h"


int merge2unit(const struct msg_header* hdr_unit, const char* data, char* unit, int unitlen)
{
    int len_unitheader = sizeof(struct msg_header);

    assert((len_unitheader + hdr_unit->numbytes) < unitlen);
#ifdef WIN32
    memcpy_s(unit, unitlen, hdr_unit, len_unitheader);
    memcpy_s(unit + len_unitheader, unitlen - len_unitheader, data, hdr_unit->numbytes);
#else
    memcpy(unit, hdr_unit, len_unitheader);
    memcpy(unit + len_unitheader, data, hdr_unit->numbytes);
#endif

    return 0;
}

void extract_msg(const char* unit, const struct msg_header** hdr_unit, const char** data)
{
    *hdr_unit = (const struct msg_header*)unit;
    *data = (const char*)(unit + sizeof(struct msg_header));
}

int extract_copy_msg(const char* unit, struct msg_header* hdr_unit, char* data, int datalen)
{
    int len_unitheader = sizeof(struct msg_header);

#ifdef WIN32
    memcpy_s(hdr_unit, len_unitheader, unit, len_unitheader);
    memcpy_s(data, datalen, unit + len_unitheader, hdr_unit->numbytes);
#else
    memcpy(hdr_unit, unit, len_unitheader);
    memcpy(data, unit + len_unitheader, hdr_unit->numbytes);
#endif

    return 0;
}

int push2pool(const char* data, const struct msg_header* unithdr, struct sendrecv_pool* pool)
{
    char* poolbuf = NULL;

    csmutex_lock(pool->hmutex);
    poolbuf = pool->empty_buf.pull_item(&pool->empty_buf);
    csmutex_unlock(pool->hmutex);

    if (poolbuf == NULL) {
        return 1;
    }

    if (merge2unit(unithdr, data, poolbuf, pool->len_item) != 0)
    {
        printf("copy data to pool error, omit current data.\n");
        csmutex_lock(pool->hmutex);
        pool->empty_buf.push_item(&pool->empty_buf, poolbuf);
        csmutex_unlock(pool->hmutex);
        return 1;
    }

    csmutex_lock(pool->hmutex);
    pool->filled_buf.push_item(&pool->filled_buf, poolbuf);
    csmutex_unlock(pool->hmutex);

    ReleaseSemaphore(pool->hsem_filled, 1, NULL);

    return 0;
}

int pull_from_pool(char* data, int datalen, struct msg_header* unithdr, struct sendrecv_pool* pool)
{
    char* bufitem = NULL;

    csmutex_lock(pool->hmutex);
    bufitem = pool->filled_buf.pull_item(&pool->filled_buf);
    csmutex_unlock(pool->hmutex);
    if (bufitem == NULL) {
        return 1;
    }

    extract_copy_msg(bufitem, unithdr, data, datalen);

    csmutex_lock(pool->hmutex);
    pool->empty_buf.push_item(&pool->empty_buf, bufitem);
    csmutex_unlock(pool->hmutex);

    return 0;
}
