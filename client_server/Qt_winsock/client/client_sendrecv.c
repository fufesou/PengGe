/**
 * @file client_sendrecv.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-03
 */

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <setjmp.h>
#include <signal.h>
#endif

#include <stdio.h>
#include <string.h>
#include "unprtt.h"
#include "udp_types.h"


#ifdef __cplusplus
extern "C" {
#endif

static struct rtt_info rttinfo;
static struct hdr sendhdr, recvhdr;
static int rttinit = 0;

static void s_sig_alarm(int signo);

#ifdef WIN32
static struct WSASendRecvMsg msgsend, msgrecv;
#else
static struct msghdr msgsend, msgrecv;
#endif

#ifdef WIN32
static MMRESULT time_event_id = 0;

#define RECV_TIMEOUT 1
#define RECV_OK 2
#define RECV_RESEND  3
static int recv_status = 0;

static void s_start_timeevent(int msec);
static void s_recvmsg_fail(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
static void s_kill_timeevent(void);
static void s_send_msg(SOCKET fd, struct WSASendRecvMsg* msg);
static void s_recv_msg(SOCKET fd, struct WSASendRecvMsg* msg);
#else
static sigjmp_buf jmpbuf;

/**
 * @todo implement later
 */
static void s_send_msg(int fd, struct msghdr* msg);
static void s_recv_msg(int fd, struct WSASendRecvMsg* msg);
#endif


ssize_t client_sendrecv(
        SOCKET fd, 
        const void* outbuf, size_t outbytes,
        void* inbuf, size_t inbytes,
        const struct sockaddr* destaddr, int destlen)
{
#ifdef WIN32
    WSABUF iovsend[2], iovrecv[2];
#else
    ssize_t n;
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
#define bufptr char*
#else
#define bufptr void*
#endif

#ifdef WIN32
    memcpy_s(&msgsend.msg_name, sizeof(msgsend.msg_name), destaddr, destlen);
#else
    memcpy(&msgsend.msg_name, destaddr, destlen);
#endif
    msgsend.msg_namelen = destlen;
    msgsend.msg_iov = iovsend;
    msgsend.msg_iovlen = 2;
    msgsend.flags = 0;
    iovsend[0].iov_base = (bufptr)&sendhdr;
    iovsend[0].iov_len = sizeof(struct hdr);
    iovsend[1].iov_base = (bufptr)outbuf;
    iovsend[1].iov_len = outbytes;

    msgrecv.msg_namelen = sizeof(msgrecv.msg_name);
    msgrecv.msg_iov = iovrecv;
    msgrecv.msg_iovlen = 2;
    msgrecv.flags = 0;
    iovrecv[0].iov_base = (bufptr)&recvhdr;
    iovrecv[0].iov_len = sizeof(struct hdr);
    iovrecv[1].iov_base = (bufptr)inbuf;
    iovrecv[1].iov_len = inbytes;

#ifdef WIN32
#undef iov_base
#undef iov_len
#undef bufptr
#endif

    rtt_newpack(&rttinfo);

    recv_status = 0;
    while (recv_status!=RECV_OK && recv_status!=RECV_TIMEOUT) {
        s_send_msg(fd, &msgsend);
        while (!recv_status) {
            s_recv_msg(fd, &msgrecv);
            if ((msgrecv.numbytes > sizeof(struct hdr)) && (recvhdr.seg == sendhdr.seg)) {
                recv_status = RECV_OK;
                s_kill_timeevent();
            }
        }
    }

    if (recv_status == RECV_TIMEOUT) {
        return (-1);
    }

    rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);

    return msgrecv.numbytes - sizeof(struct hdr);
}

void s_sig_alarm(int signo)
{
    (void)signo;
#ifndef WIN32
    longjmp(jmpbuf, 1);
#endif
}

#ifdef WIN32
void s_recvmsg_fail(UINT timer_id, UINT msg, DWORD_PTR user_data, DWORD_PTR dw1, DWORD_PTR dw2)
{
    (void)timer_id;
    (void)msg;
    (void)user_data;
    (void)dw1;
    (void)dw2;

    if (rtt_timeout(&rttinfo) < 0) {
        printf("no response from sever, giving up");
        rttinit = 0;
        recv_status = RECV_TIMEOUT;
    }
    else {
        recv_status = RECV_RESEND;
    }
}

void s_start_timeevent(int msec)
{
    time_event_id = timeSetEvent(msec, 500, (LPTIMECALLBACK)s_recvmsg_fail, 0, TIME_CALLBACK_FUNCTION | TIME_ONESHOT);
}

void s_kill_timeevent(void)
{
    if (time_event_id) {
        timeKillEvent(time_event_id);
        time_event_id = 0;
    }
}

void s_send_msg(SOCKET fd, struct WSASendRecvMsg* msg)
{
    sendhdr.ts = rtt_ts(&rttinfo);
    if (0 != WSASendTo(
                fd,
                msg->msg_iov,
                msg->msg_iovlen,
                &msg->numbytes,
                msg->flags,
                (void*)&msg->msg_name,
                msg->msg_namelen,
                NULL,
                NULL))
    {
        printf("s_send_msg error.\n");
    }
    recv_status = 0;
    s_start_timeevent(rtt_start(&rttinfo) * 1000);
}

void s_recv_msg(SOCKET fd, struct WSASendRecvMsg* msg)
{
    if (0 != WSARecvFrom(
                fd,
                msg->msg_iov,
                msg->msg_iovlen,
                &msg->numbytes,
                &msg->flags,
                (void*)&msg->msg_name,
                &msg->msg_namelen,
                NULL,
                NULL))
    {
        printf("s_recv_msg error.\n");
        msg->numbytes = 0;
    }
}

#endif

#ifdef __cplusplus
}
#endif
