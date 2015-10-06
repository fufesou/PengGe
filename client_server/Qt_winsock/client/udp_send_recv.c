/**
 * @file udp_send_recv.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-03
 */

#include "unprtt.h"
#include <winsock2.h>
#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif

static struct rtt_info rttinfo;
static int rttinit = 0;

#ifndef WIN32
static struct msghdr msgsend, msgrecv;
#endif

static struct hdr {
    uint32_t seg;
    uint32_t ts;
} sendhdr, recvhdr;

static void sig_alarm(int signo);

#ifdef WIN32
static jmp_buf jmpbuf;
#else
static sigjmp_buf jmpbuf;
#endif

#ifdef WIN32
static struct WSASendToMsg {
    LPWSABUF msg_iov;
    DWORD msg_iovlen;
    LPWORD numbytes;
    DWORD flags;
    struct sockaddr* msg_name;
    int msg_namelen;
}msgsend, msgrecv;
#else
static struct msghdr msgsend, msgrecv;
#endif

ssize_t U_send_recv(
        SOCKET fd, 
        const void* outbuf, size_t outbytes,
        void* inbuf, size_t inbytes,
        const struct sockaddr* destaddr, int destlen)
{
    ssize_t n;
#ifdef WIN32
    LPWSABUF iovsend[2], iovrecv[2];
#else
    struct iovec iovsend[2], iovrecv[2];
#endif
    if (rttinit == 0) {
        rtt_init(&rttinfo);
        rttinit = 1;
        rtt_d_flag = 1;
    }

    ++sendhdr.seg;

#ifdef WIN32
#define iov_base buf
#define iov_len len
#endif

    msgsend.msg_name = destaddr;
    msgsend.msg_namelen = destlen;
    msgsend.msg_lpbuf = iovsend;
    msgsend.msg_lpbuflen = 2;
    iovsend[0]->iov_base = &sendhdr;
    iovsend[0]->iov_len = sizeof(struct hdr);
    iovsend[1]->iov_base = outbuf;
    iovsend[1]->iov_len = outbytes;

    msgrecv.msg_name = NULL;
    msgrecv.msg_namelen = 0;
    msgrecv.msg_iov = iovrecv;
    msgrecv.msg_iovlen = 2;
    msgrecv[0].iov_base = &recvhdr;
    msgrecv[0].iov_len = sizeof(struct hdr);
    msgrecv[1].iov_base = inbuf;
    msgrecv[1].iov_len = inbytes;

#ifdef WIN32
#ifdef iov_base
#undef iov_base
#endif
#ifdef iov_len
#undef iov_len
#endif
}


#ifdef __cplusplus
}
#endif
