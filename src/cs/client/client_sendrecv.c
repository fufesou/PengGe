/**
 * @file client_sendrecv.c
 * @brief This file provide the windows version of client sendrecv.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-03
 * @modified  Wed 2015-12-23 22:15:40 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#include  <process.h>
#else
#include  <unistd.h>
#include  <errno.h>
#include  <sys/socket.h>
#include  <arpa/inet.h>
#include  <pthread.h>

#define SOCKET_ERROR (-1)
#endif

#ifndef _MSC_VER /* *nix */
#include  <semaphore.h>
#endif

#include  <stdio.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/jxiot.h"
#include    "common/timespan.h"
#include    "common/list.h"
#include    "common/clearlist.h"
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "common/sock_wrap.h"
#include    "common/bufarray.h"
#include    "common/msgwrap.h"
#include    "common/utility_wrap.h"
#include    "cs/msgpool.h"
#include    "cs/unprtt.h"
#include    "cs/msgpool_dispatch.h"
#include    "cs/client.h"


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

static int s_mutex_inited = 0;
static csmutex_t s_mutex;
static struct csmsg_header s_sendhdr;

static int s_recvmsg(cssock_t hsock, void* inmsg, size_t inbytes);
static void s_reset_recvtimer(void);
static int s_recv_elapsed(void);

void s_sendrecv_init(void);
void s_sendrecv_clear(void* unused);


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

    if (s_mutex_inited == 0) {
        s_sendrecv_init();
    }

    ++s_sendhdr.header.seq;
    rtt_newpack(&s_rttinfo);
    sendlen = sizeof(s_sendhdr.addr);
    cssock_getsockname(cli->hsock, &s_sendhdr.addr, &sendlen);
    s_sendhdr.addrlen = (uint8_t)sendlen;

    s_recv_stat = RECV_RESEND;
    while (RECV_RESEND == s_recv_stat) {
        s_sendhdr.header.ts = rtt_ts(&s_rttinfo);
        s_sendhdr.numbytes = htonl(cli->len_senddata);
        csmsg_merge(&s_sendhdr, cli->sendbuf, outbuf, sizeof(outbuf));

        if (SOCKET_ERROR == sendto(
                    cli->hsock,
                    outbuf,
                    sizeof(struct csmsg_header) + ntohl(s_sendhdr.numbytes),
                    0,
                    servaddr,
                    (cssocklen_t)addrlen)) {
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
                fprintf(stderr, "recvfrom() fail, error code: %d.\n", errcode);
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

void s_sendrecv_init(void)
{
    if (s_mutex_inited == 0) {
        s_mutex = csmutex_create();
    }

    csclearlist_add(s_sendrecv_clear, NULL);
}

void s_sendrecv_clear(void* unused)
{
    (void)unused;
    csmutex_destroy(&s_mutex);
}
