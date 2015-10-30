/**
 * @file client_sendrecv.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-03
 * @modified  Fri 2015-10-30 18:42:24 (+0800)
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
#include "sock_types.h"


#ifdef __cplusplus
extern "C" {
#endif

static int rttinit = 0;
static struct rtt_info rttinfo;
static struct hdr sendhdr, recvhdr;

#ifndef WIN32
static void s_sig_alarm(int signo);
#endif

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
static int status_accesible = 1;
CRITICAL_SECTION s_cscode;

static int s_initWSASendRecvMsg(
        struct WSASendRecvMsg* msg,
        LPWSABUF pwsabuf,
        int bufnum,
        struct hdr* buf0,
        char* buf1,
        int buf1len,
        const struct sockaddr* peeraddr,
        int addrlen);

static void s_start_timeevent(int msec);
static void s_recvmsg_fail(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
static void s_kill_timeevent(void);
static void s_send_msg(SOCKET fd, struct WSASendRecvMsg* msg);
static int s_recv_msg(SOCKET fd, struct WSASendRecvMsg* msg);
#else
static sigjmp_buf jmpbuf;

/**
 * @todo implement later
 */
static void s_send_msg(int fd, struct msghdr* msg);
static void s_recv_msg(int fd, struct msghdr* msg);
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

    s_initWSASendRecvMsg(
                &msgsend,
                iovsend,
                2,
                &sendhdr,
                (char*)outbuf,
                outbytes,
                destaddr,
                destlen);
    s_initWSASendRecvMsg(
                &msgrecv,
                iovrecv,
                2,
                &recvhdr,
                inbuf,
                inbytes,
                NULL,
                sizeof(msgrecv.msg_name));

    rtt_newpack(&rttinfo);
    InitializeCriticalSection(&s_cscode);

    recv_status = RECV_RESEND;
    status_accesible = 1;
    while (recv_status!=RECV_OK && recv_status!=RECV_TIMEOUT) {
        if (recv_status == RECV_RESEND)
            s_send_msg(fd, &msgsend);
        while (!recv_status) {
            s_recv_msg(fd, &msgrecv);

            EnterCriticalSection(&s_cscode);
            if ((msgrecv.numbytes > sizeof(struct hdr)) && (recvhdr.seg == sendhdr.seg)) {
                recv_status = RECV_OK;
                s_kill_timeevent();
            } else {
                status_accesible = 1;
            }
            LeaveCriticalSection(&s_cscode);
        }
    }

    if (recv_status == RECV_TIMEOUT) {
        return (-1);
    }

    rtt_stop(&rttinfo, rtt_ts(&rttinfo) - recvhdr.ts);
    DeleteCriticalSection(&s_cscode);

    return msgrecv.numbytes - sizeof(struct hdr);
}

#ifndef WIN32
void s_sig_alarm(int signo)
{
    (void)signo;
    longjmp(jmpbuf, 1);
}
#endif

#ifdef WIN32
int s_initWSASendRecvMsg(
        struct WSASendRecvMsg* msg,
        LPWSABUF pwsabuf,
        int bufnum,
        struct hdr* buf0,
        char* buf1,
        int buf1len,
        const struct sockaddr* peeraddr,
        int addrlen)
{
    if (peeraddr != NULL) {
#ifdef WIN32
    memcpy_s(&msg->msg_name, sizeof(msg->msg_name), peeraddr, addrlen);
#else
    memcpy(&msg->msg_name, destaddr, destlen);
#endif
    }
    msg->msg_namelen = addrlen;
    msg->msg_iov = pwsabuf;
    msg->msg_iovlen = bufnum;
    msg->flags = 0;
    pwsabuf[0].buf = (char*)buf0;
    pwsabuf[0].len = sizeof(struct hdr);
    pwsabuf[1].buf = buf1;
    pwsabuf[1].len = buf1len;

    SecureZeroMemory((PVOID)&msg->overlapped, sizeof(WSAOVERLAPPED));
    msg->overlapped.hEvent = WSACreateEvent();
    if (msg->overlapped.hEvent == NULL) {
        printf("WSACreateEvent failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    return 0;
}

/**
 * @brief s_recvmsg_fail is a callback function that will be called when assigned time elapsed.
 * This function can set the 'recv_status' value.
 * @note This function can read 'status_accesible' value, however it cannot change 'status_accesible'.
 */
void s_recvmsg_fail(UINT timer_id, UINT msg, DWORD_PTR user_data, DWORD_PTR dw1, DWORD_PTR dw2)
{
    (void)timer_id;
    (void)msg;
    (void)user_data;
    (void)dw1;
    (void)dw2;

    EnterCriticalSection(&s_cscode);
    if (!status_accesible) {
        s_start_timeevent(100);
        LeaveCriticalSection(&s_cscode);
        return;
    }

    if (rtt_timeout(&rttinfo) < 0) {
        printf("no response from sever, giving up");
        rttinit = 0;
        recv_status = RECV_TIMEOUT;
    }
    else {
        recv_status = RECV_RESEND;
    }
    LeaveCriticalSection(&s_cscode);
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

int s_recv_msg(SOCKET fd, struct WSASendRecvMsg* msg)
{
    /**
     * @brief The following function is blocked, because the last two parameter is NULL.
     *
     * @todo make the following function none blocked later.
     */
    int ret_recv = WSARecvFrom(
                            fd,
                            msg->msg_iov,
                            msg->msg_iovlen,
                            &msg->numbytes,
                            &msg->flags,
                            (void*)&msg->msg_name,
                            &msg->msg_namelen,
                            NULL,
                            NULL);
    EnterCriticalSection(&s_cscode);
    if (ret_recv != 0) {
        printf("s_recv_msg error.\n");
        if (10054 == WSAGetLastError()) {
            printf("cannot connect to peer device.\n" );
        }
        msg->numbytes = 0;
    }
    status_accesible = 0;
    LeaveCriticalSection(&s_cscode);
    return 0;
}

#endif

#ifdef __cplusplus
}
#endif
