/**
 * @file udp_send_recv.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-03
 */

#include "unprtt.h"
#include <stdio.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <setjmp.h>
#include <signal.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static struct rtt_info rttinfo;
static int rttinit = 0;

static struct hdr {
    uint32_t seg;
    uint32_t ts;
} sendhdr, recvhdr;

static void sig_alarm(int signo);

#ifdef WIN32
static struct WSASendRecvMsg {
    LPWSABUF msg_iov;
    DWORD msg_iovlen;
    DWORD numbytes;
    DWORD flags;
    struct sockaddr* msg_name;
    int msg_namelen;
}msgsend, msgrecv;
#else
static struct msghdr msgsend, msgrecv;
#endif

#ifdef WIN32
static MMRESULT time_event_id = 0;

#define RECV_TIMEOUT 1
#define RECV_OK 2
#define RECV_RESEND  3
static int recv_status = 0;

static void start_time_event(int msec);
static void recv_msg_fail(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
static void kill_time_event(void);
static void send_msg(SOCKET fd, struct WSASendRecvMsg* msg);
static void recv_msg(SOCKET fd, struct WSASendRecvMsg* msg);
#else
static sigjmp_buf jmpbuf;

/**
 * @todo implement later
 */
static void send_msg(SOCKET fd, struct msghdr* msg);
#endif


ssize_t U_send_recv(
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

    msgsend.msg_name = (struct sockaddr*)destaddr;
    msgsend.msg_namelen = destlen;
    msgsend.msg_iov = iovsend;
    msgsend.msg_iovlen = 2;
    msgsend.flags = 0;
    iovsend[0].iov_base = (bufptr)&sendhdr;
    iovsend[0].iov_len = sizeof(struct hdr);
    iovsend[1].iov_base = (bufptr)outbuf;
    iovsend[1].iov_len = outbytes;

    msgrecv.msg_name = NULL;
    msgrecv.msg_namelen = 0;
    msgrecv.msg_iov = iovrecv;
    msgrecv.msg_iovlen = 2;
    msgsend.flags = 0;
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

    while (recv_status!=RECV_OK && recv_status!=RECV_TIMEOUT) {
        send_msg(fd, &msgsend);
        while (!recv_status) {
            recv_msg(fd, &msgrecv);
            if ((msgrecv.numbytes > sizeof(struct hdr)) && (recvhdr.seg == sendhdr.seg)) {
                recv_status = RECV_OK;
                kill_time_event();
            }
        }
    }

    if (recv_status == RECV_TIMEOUT) {
        return (-1);
    }

    rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);

    return msgrecv.numbytes - sizeof(struct hdr);
}

void sig_alarm(int signo)
{
    (void)signo;
#ifndef WIN32
    longjmp(jmpbuf, 1);
#endif
}

#ifdef WIN32
void recv_msg_fail(UINT timer_id, UINT msg, DWORD_PTR user_data, DWORD_PTR dw1, DWORD_PTR dw2)
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

void start_time_event(int msec)
{
    time_event_id = timeSetEvent(msec, 500, (LPTIMECALLBACK)recv_msg_fail, 0, TIME_CALLBACK_FUNCTION | TIME_ONESHOT);
}

void kill_time_event(void)
{
    if (time_event_id) {
        timeKillEvent(time_event_id);
        time_event_id = 0;
    }
}

void send_msg(SOCKET fd, struct WSASendRecvMsg* msg)
{
    sendhdr.ts = rtt_ts(&rttinfo);
    if (0 != WSASendTo(fd, msg->msg_iov, msg->msg_iovlen, &msg->numbytes, msg->flags, msg->msg_name, msg->msg_namelen, NULL, NULL)) {
        printf("send_msg error.\n");
    }
    recv_status = 0;
    start_time_event(rtt_start(&rttinfo) * 1000);
}

void recv_msg(SOCKET fd, struct WSASendRecvMsg* msg)
{
    if (0 != WSARecvFrom(fd, msg->msg_iov, msg->msg_iovlen, &msg->numbytes, &msg->flags, msg->msg_name, &msg->msg_namelen, NULL, NULL))
    {
        printf("send_msg error.\n");
        msg->numbytes = 0;
    }
}

#endif

#ifdef __cplusplus
}
#endif
