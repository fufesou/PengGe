/**
 * @file msgwrap.c
 * @brief length check is required for memcpy in this file.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  周五 2015-11-06 12:04:44 中国标准时间
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
#include    "cstypes.h"
#include    "macros.h"
#include    "utility_wrap.h"
#include	"sock_types.h"
#include    "lightthread.h"
#include    "bufarray.h"
#include    "msgpool.h"
#include    "msgwrap.h"


int csmsg_copyaddr(struct csmsg_header* msghdr, const struct sockaddr* addr, int addrlen)
{
    if (cs_memcpy(&msghdr->addr, sizeof(msghdr->addr), addr, addrlen) != 0) {
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
