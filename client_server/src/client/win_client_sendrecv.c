/**
 * @file client_sendrecv.c
 * @brief This file provide the windows version of client sendrecv.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-03
 * @modified  周五 2015-11-06 12:14:19 中国标准时间
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
#include    "timespan.h"
#include    "unprtt.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "sock_wrap.h"
#include    "bufarray.h"
#include    "msgpool.h"
#include    "msgwrap.h"
#include    "utility_wrap.h"
#include    "client.h"


#ifdef __cplusplus
extern "C" {
#endif

#define RTT_DEBUG

static struct rtt_info s_rttinfo;
static int s_rttinit = 0;

#define RECV_TIMEOUT -1
#define RECV_RESEND 1
#define RECV_OK 0

static int s_recv_stat = RECV_RESEND;
static cstimelong_t s_recvbegin;
static int s_recvtimer;

static csmutex_t s_mutex = 0;
static struct csmsg_header s_sendhdr;

static int s_recvmsg(cssock_t hsock, void* inmsg, size_t inbytes);
static void s_reset_recvtimer(void);
static int s_recv_elapsed(void);

#ifdef __cplusplus
}
#endif

ssize_t csclient_sendrecv(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
	char outbuf[MAX_MSG_LEN];
    ssize_t recvbytes;
    cssocklen_t sendlen;

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
    cssock_getsockname(cli->hsock, &s_sendhdr.addr, &sendlen);
    s_sendhdr.addrlen = sendlen;

    s_recv_stat = RECV_RESEND;
    while (RECV_RESEND == s_recv_stat) {
        s_sendhdr.header.ts = rtt_ts(&s_rttinfo);
        s_sendhdr.numbytes = strlen(cli->sendbuf) + 1;
        csmsg_merge(&s_sendhdr, cli->sendbuf, outbuf, sizeof(outbuf));
        if (SOCKET_ERROR == sendto(cli->hsock, outbuf, sizeof(struct csmsg_header) + s_sendhdr.numbytes, 0, servaddr, addrlen)) {
            fprintf(stderr, "%s sendto() fail, error code: %d.\n", cli->prompt, cssock_get_last_error());
            return -2;
        }

        s_reset_recvtimer();
        if ((recvbytes = s_recvmsg(cli->hsock, cli->recvbuf, sizeof(cli->recvbuf))) == -3) {
            return -3;
        }
    }

    if (s_recv_stat == RECV_TIMEOUT) {
        s_rttinit = 0;
        return -1;
    }

    rtt_stop(&s_rttinfo, rtt_ts(&s_rttinfo) - ((struct csmsg_header*)cli->recvbuf)->header.ts);
    csmsg_copyaddr((struct csmsg_header*)cli->recvbuf, servaddr, addrlen);

    return recvbytes - sizeof(struct csmsg_header);
}

int s_recvmsg(cssock_t hsock, void* inmsg, size_t inbytes)
{
    int errcode;
    ssize_t recvbytes = -1;

    while (1) {
        if (s_recv_elapsed()) {
            if (rtt_timeout(&s_rttinfo) < 0) {
                printf("no response from server, giving up.\n");
                s_recv_stat = RECV_TIMEOUT;
            } else {
                s_recv_stat = RECV_RESEND;
            }
            return -1;
        }

        if ((recvbytes = recvfrom(hsock, inmsg, inbytes, 0, NULL, NULL)) == -1) {
            errcode = cssock_get_last_error();
            if (ETRYAGAIN(errcode)) {
                continue;
            } else {
                fprintf(stderr, "recvfrom() fail, error code: %d", errcode);
                return -3;
            }
        } else if (recvbytes > (ssize_t)sizeof(struct csmsg_header) &&
            ((struct csmsg_header*)inmsg)->header.seq == s_sendhdr.header.seq) {

            s_recv_stat = RECV_OK;
            return recvbytes;
        }

    }
}

void s_reset_recvtimer(void)
{
    cstimelong_cur(&s_recvbegin);
    s_recvtimer = rtt_start(&s_rttinfo);
}

int s_recv_elapsed(void)
{
    s_recvtimer -= (int)cstimelong_span_sec(&s_recvbegin);
    return (s_recvtimer <= 0);
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
