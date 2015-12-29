/**
 * @file msgwrap.c
 * @brief length check is required for memcpy in this file.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Sun 2015-12-06 18:20:44 (+0800)
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
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/utility_wrap.h"
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "common/bufarray.h"
#include    "common/msgwrap.h"
#include    "cs/msgpool.h"


int csmsg_copyaddr(struct csmsg_header* msghdr, const struct sockaddr* addr, uint8_t addrlen)
{
    if (cs_memcpy(&msghdr->addr, sizeof(msghdr->addr), addr, (size_t)addrlen) != 0) {
        return 1;
    }
    msghdr->addrlen = addrlen;
    return 0;
}

int csmsg_merge(const struct csmsg_header* msgheader, const char* data, char* unit, uint32_t unitlen)
{
    uint32_t len_unitheader = sizeof(struct csmsg_header);
    uint32_t msgdatalen = ntohl(msgheader->numbytes);

    assert((len_unitheader + msgdatalen) < unitlen);
    if (cs_memcpy(unit, unitlen, msgheader, len_unitheader) != 0) {
        return 1;
    }
    if (cs_memcpy(unit + len_unitheader, unitlen - len_unitheader, data, msgdatalen) != 0) {
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
    
    if (cs_memcpy(data, datalen, unit + len_unitheader, ntohl(msgheader->numbytes)) != 0) {
        return 1;
    }
    
    return 0;
}
