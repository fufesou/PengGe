/**
 * @file msgwrap.c
 * @brief length check is required for memcpy in this file.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
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


int jxmsg_copyaddr(struct jxmsg_header* msghdr, const struct sockaddr* addr, uint8_t addrlen)
{
    if (jxmemcpy(&msghdr->addr, sizeof(msghdr->addr), addr, (size_t)addrlen) != 0) {
        return 1;
    }
    msghdr->addrlen = addrlen;
    return 0;
}

int jxmsg_merge(const struct jxmsg_header* msgheader, const char* data, char* unit, uint32_t unitlen)
{
    uint32_t len_unitheader = sizeof(struct jxmsg_header);
    uint32_t msgdatalen = ntohl(msgheader->numbytes);

    assert((len_unitheader + msgdatalen) < unitlen);
    if (jxmemcpy(unit, unitlen, msgheader, len_unitheader) != 0) {
        return 1;
    }
    if (jxmemcpy(unit + len_unitheader, unitlen - len_unitheader, data, msgdatalen) != 0) {
        return 1;
    }

    return 0;
}

void jxmsg_extract(const char* unit, const struct jxmsg_header** msgheader, const char** data)
{
    *msgheader = (const struct jxmsg_header*)unit;
    *data = (const char*)(unit + sizeof(struct jxmsg_header));
}

int jxmsg_extract_copy(const char* unit, struct jxmsg_header* msgheader, char* data, int datalen)
{
    int len_unitheader = sizeof(struct jxmsg_header);

    if (jxmemcpy(msgheader, len_unitheader, unit, len_unitheader) != 0) {
        return 1;
    }
    
    if (jxmemcpy(data, datalen, unit + len_unitheader, ntohl(msgheader->numbytes)) != 0) {
        return 1;
    }
    
    return 0;
}
