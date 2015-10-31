/**
 * @file server_sendrecv.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-10
 */


#include <string.h>
#include <stdio.h>
#include <stdint.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "sock_types.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifdef WIN32
static struct WSASendRecvMsg msgsend, msgrecv;
#else
static struct msghdr msgsend, msgrecv;
#endif

#ifdef WIN32
static void s_send_msg(SOCKET fd, struct WSASendRecvMsg* msg);
static ssize_t s_recv_msg(SOCKET fd, struct WSASendRecvMsg* msg);
#endif

ssize_t server_recv(
        SOCKET fd,
        void* inbuf,
        size_t inbytes,
        struct hdr* hdrdata,
        struct sockaddr* cli_addr,
        int* cli_addrlen)
{
    ssize_t recvbytes;
#ifdef WIN32
    WSABUF iovrecv[2];
#else
    struct iovec iovrecv[2];
#endif

    msgrecv.msg_namelen = sizeof(msgrecv.msg_name);
    msgrecv.msg_iov = iovrecv;
    msgrecv.msg_iovlen = 2;
    msgrecv.flags = 0;

#ifdef WIN32
#define iov_base buf
#define iov_len len
#define bufptr char*
#else
#define bufptr void*
#endif
    iovrecv[0].iov_base = (bufptr)hdrdata;
    iovrecv[0].iov_len = sizeof(struct hdr);
    iovrecv[1].iov_base = (bufptr)inbuf;
    iovrecv[1].iov_len = inbytes;
#ifdef WIN32
#undef iov_base
#undef iov_len
#undef bufptr
#endif

    recvbytes = s_recv_msg(fd, &msgrecv);

    /**
     * @todo This call may be not safe without error checking.
     */
#ifdef WIN32
    memcpy_s(cli_addr, *cli_addrlen, &msgrecv.msg_name, msgrecv.msg_namelen);
#else
    memcpy(cli_addr, &msgrecv.msg_name, msgrecv.msg_namelen);
#endif
    *cli_addrlen = msgrecv.msg_namelen;

    return recvbytes - sizeof(struct hdr);
}

void server_send(
        SOCKET fd,
        const void* outbuf,
        size_t outbytes,
        const struct hdr* hdrdata,
        const struct sockaddr* cli_addr,
        int cli_addrlen)
{
#ifdef WIN32
    WSABUF iovsend[2];
#else
    ssize_t n;
    struct iovec iovsend[2], iovrecv[2];
#endif

#ifdef WIN32
    memcpy_s(&msgsend.msg_name, sizeof(msgsend.msg_name), cli_addr, cli_addrlen);
#else
    memcpy(&msgsend.msg_name, cli_addr, cli_addrlen);
#endif

    msgsend.msg_namelen = cli_addrlen;
    msgsend.msg_iov = iovsend;
    msgsend.msg_iovlen = 2;
    msgsend.flags = 0;

#ifdef WIN32
#define iov_base buf
#define iov_len len
#define bufptr char*
#else
#define bufptr void*
#endif
    iovsend[0].iov_base = (bufptr)hdrdata;
    iovsend[0].iov_len = sizeof(struct hdr);
    iovsend[1].iov_base = (bufptr)outbuf;
    iovsend[1].iov_len = outbytes;
#ifdef WIN32
#undef iov_base
#undef iov_len
#undef bufptr
#endif

    s_send_msg(fd, &msgsend);
}

#ifdef WIN32
void s_send_msg(SOCKET fd, struct WSASendRecvMsg* msg)
{
    if (0 != WSASendTo(
                fd,
                msg->msg_iov,
                msg->msg_iovlen,
                &msg->numbytes,
                msg->flags,
                (struct sockaddr*)&msg->msg_name,
                msg->msg_namelen,
                NULL,
                NULL))
    {
        fputs("s_send_msg error.\n", stderr);
    }
}

ssize_t s_recv_msg(SOCKET fd, struct WSASendRecvMsg* msg)
{
    if (0 != WSARecvFrom(
                fd,
                msg->msg_iov,
                msg->msg_iovlen,
                &msg->numbytes,
                &msg->flags,
                (struct sockaddr*)&msg->msg_name,
                &msg->msg_namelen,
                NULL,
                NULL))
    {
        fputs("s_recv_msg error.\n", stderr);
        msg->numbytes = 0;
        return -1;
    }
    return msg->numbytes;
}

#endif

#ifdef __cplusplus
}
#endif
