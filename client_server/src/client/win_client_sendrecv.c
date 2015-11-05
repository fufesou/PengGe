/**
 * @file client_sendrecv.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-03
 * @modified  2015-11-04 22:31:14 (周三)
 */

#ifdef WIN32

#include  <winsock2.h>
#include  <windows.h>
#include  <process.h>
#include  <semaphore.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <string.h>
#include    "macros.h"
#include    "unprtt.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "sock_wrap.h"
#include    "bufarray.h"
#include    "sendrecv_pool.h"
#include    "msgwrap.h"
#include    "utility_wrap.h"
#include    "client.h"


#ifdef __cplusplus
extern "C" {
#endif

#define RTT_DEBUG

#define RECV_TIMEOUT 1
#define RECV_OK 2
#define RECV_RESEND 3
static int s_recv_stat = 0;

static struct rtt_info s_rttinfo;
static int s_rttinit = 0;

static int s_event_timeout = 0;

static csmutex_t s_mutex = 0;
static struct csmsg_header s_sendhdr;
static MMRESULT s_timeevent = 0;

static int s_recvmsg(cssock_t hsock, void* inmsg, size_t inbytes);
static void s_recvmsg_fail(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
static void s_start_timeevent(int msec);
static void s_kill_timeevent(void);

#ifdef __cplusplus
}
#endif

ssize_t csclient_sendrecv(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
	char outbuf[MAX_MSG_LEN];
    ssize_t recvbytes;

    if (s_rttinit == 0) {
        rtt_init(&s_rttinfo);
        s_rttinit = 1;
#ifdef _DEBUG
        rtt_d_flag = 1;
#endif
    }

    if (!s_mutex) {
        cssendrecv_init();
    }

    ++s_sendhdr.header.seq;
    rtt_newpack(&s_rttinfo);

    s_recv_stat = RECV_RESEND;
    while (s_recv_stat != RECV_OK && s_recv_stat != RECV_TIMEOUT) {
        if (s_recv_stat == RECV_RESEND) {
            s_sendhdr.header.ts = rtt_ts(&s_rttinfo);
            s_sendhdr.numbytes = strlen(cli->sendbuf) + 1;
            csmsg_merge(&s_sendhdr, cli->sendbuf, outbuf, sizeof(outbuf));
            if (SOCKET_ERROR == sendto(cli->hsock, outbuf, sizeof(struct csmsg_header) + s_sendhdr.numbytes, 0, servaddr, addrlen)) {
                fprintf(stderr, "%s sendto() fail, error code: %d.\n", cli->msgheader, cssock_get_last_error());
                return -2;
            }

            s_recv_stat = 0;
            s_start_timeevent(rtt_start(&s_rttinfo));
        }
        if ((recvbytes = s_recvmsg(cli->hsock, cli->recvbuf, sizeof(cli->recvbuf))) == -3) {
            return -3;
        }
    }

    if (s_recv_stat == RECV_TIMEOUT) {
        s_rttinit = 0;
        return -1;
    }

    rtt_stop(&s_rttinfo, rtt_ts(&s_rttinfo) - ((struct csmsg_header*)cli->recvbuf)->header.ts);

    return recvbytes - sizeof(struct csmsg_header);
}

int s_recvmsg(cssock_t hsock, void* inmsg, size_t inbytes)
{
    int errcode;
    ssize_t recvbytes = -1;

    while (1) {
        if (s_event_timeout) {
            if (rtt_timeout(&s_rttinfo) < 0) {
            printf("no response from server, giving up.\n");
            s_recv_stat = RECV_TIMEOUT;
            } else {
                s_recv_stat = RECV_RESEND;
            }
            return -1;
        }

        if ((recvbytes = recvfrom(hsock, inmsg, inbytes, 0, NULL, NULL)) == -1) {
            if ((errcode = cssock_get_last_error()) == WSAEWOULDBLOCK) {
                printf("non-blocking option is set, recve again.\n");
                continue;
            } else {
                fprintf(stderr, "recvfrom() fail, error code: %d", errcode);
                s_kill_timeevent();
                return -3;
            }
        } else if (recvbytes > (ssize_t)sizeof(struct csmsg_header) &&
            ((struct csmsg_header*)inmsg)->header.seq == s_sendhdr.header.seq) {

            s_recv_stat = RECV_OK;
            s_kill_timeevent();
            return recvbytes;
        }

    }
}

void s_start_timeevent(int msec)
{
    s_timeevent = timeSetEvent(msec, 500, (LPTIMECALLBACK)s_recvmsg_fail, 0, TIME_CALLBACK_FUNCTION | TIME_ONESHOT);
    s_event_timeout = 0;
}

void s_recvmsg_fail(UINT timer_id, UINT msg, DWORD_PTR user_data, DWORD_PTR dw1, DWORD_PTR dw2)
{
    (void)timer_id;
    (void)msg;
    (void)user_data;
    (void)dw1;
    (void)dw2;

    s_event_timeout = 1;
}

void s_kill_timeevent()
{
    if (s_timeevent) {
        timeKillEvent(s_timeevent);
        s_timeevent = 0;
    }
}

void cssendrecv_init(void)
{
    if (s_mutex == 0) {
        s_mutex = csmutex_create();
    }
}

void cssendrecv_clear(void)
{
    if (s_mutex != 0) {
        csmutex_destroy(s_mutex);
        s_mutex = 0;
    }
}

#endif
