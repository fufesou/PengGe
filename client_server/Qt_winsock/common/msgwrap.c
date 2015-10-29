/**
 * @file msgwrap.c
 * @brief length check is required for memcpy in this file.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  周四 2015-10-29 19:13:43 中国标准时间
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


int merge2unit(const struct msg_header* hdr_unit, const char* data, char* unit, int unitlen)
{
    int len_unitheader = sizeof(struct msg_header);

    assert((len_unitheader + hdr_unit->numbytes) < unitlen);
    cs_memcpy(unit, unitlen, hdr_unit, len_unitheader);
    cs_memcpy(unit + len_unitheader, unitlen - len_unitheader, data, hdr_unit->numbytes);

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

    if (cs_memcpy(hdr_unit, len_unitheader, unit, len_unitheader) != 0) {
        return 1;
    }
    
    if (cs_memcpy(data, datalen, unit + len_unitheader, hdr_unit->numbytes) != 0) {
        return 1;
    }
    
    return 0;
}

int push2pool(const char* data, const struct msg_header* unithdr, struct sendrecv_pool* pool)
{
    char* bufitem = cspool_pullitem(pool, &pool->empty_buf);

    if (bufitem == NULL) {
        return -1;
    }

    if (merge2unit(unithdr, data, bufitem, pool->len_item) != 0) {
        printf("copy data to pool error, omit current data.\n");
        cspool_pushitem(pool, &pool->empty_buf, bufitem);
        return 1;
    }

    cspool_pushitem(pool, &pool->filled_buf, bufitem);
    return 0;
}

int pull_from_pool(char* data, int datalen, struct msg_header* unithdr, struct sendrecv_pool* pool)
{
    char* bufitem = cspool_pullitem(pool, &pool->filled_buf);

    if (bufitem == NULL) {
        return -1;
    }

    if (extract_copy_msg(bufitem, unithdr, data, datalen) != 0) {
        fprintf(stderr, "copy data from pool error, cancel.\n");
        cspool_pushitem(pool, &pool->filled_buf, bufitem);
        return 1;
    }

    cspool_pushitem(pool, &pool->empty_buf, bufitem);
    return 0;
}
