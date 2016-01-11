/**
 * @file client_sendrecv.c
 * @brief This file provide the windows version of client sendrecv.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-03
 * @modified  Wed 2016-01-06 22:51:18 (+0800)
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


static struct rtt_info s_rttinfo;
static int s_rttinit = 0;

#define RECV_TIMEOUT -1
#define RECV_RESEND 1
#define RECV_OK 0

static int s_recv_stat = RECV_RESEND;
static jxtimelong_t s_recvbegin;
static int s_recvtimer;

static int s_mutex_inited = 0;
static jxmutex_t s_mutex;
static struct hdr s_sendhdr;

static int s_recvmsg(jxsock_t hsock, void* inmsg, size_t inbytes);
static void s_reset_recvtimer(void);
static int s_recv_elapsed(void);

void s_sendrecv_init(void);
void s_sendrecv_clear(void* unused);


#ifdef __cplusplus
}
#endif

ssize_t jxclient_sendrecv(struct jxclient* cli, const struct sockaddr* servaddr, jxsocklen_t addrlen, mflag_t mflag)
{
    char outbuf[MAX_MSG_LEN];
    ssize_t recvbytes;
    jxsocklen_t sendlen;
    struct jxmsg_header* bufheader = NULL;

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

    bufheader = (struct jxmsg_header*)outbuf;

    bufheader->mflag = mflag;
    bufheader->header.seq = ++s_sendhdr.seq;
    rtt_newpack(&s_rttinfo);
    sendlen = sizeof(bufheader->addr);
    jxsock_getsockname(cli->hsock_sendrecv, &bufheader->addr, &sendlen);
    bufheader->addrlen = (uint8_t)sendlen;
    bufheader->numbytes = htonl(cli->len_senddata);

    if (jxmemcpy(
                    outbuf + sizeof(struct jxmsg_header),
                    sizeof(outbuf) - sizeof(struct jxmsg_header),
                    cli->sendbuf, cli->len_senddata) != 0) {
        return 1;
    }

    s_recv_stat = RECV_RESEND;

    while (RECV_RESEND == s_recv_stat) {
        bufheader->header.ts = s_sendhdr.ts = rtt_ts(&s_rttinfo);

        if (SOCKET_ERROR == sendto(
                    cli->hsock_sendrecv,
                    outbuf,
                    sizeof(struct jxmsg_header) + cli->len_senddata,
                    0,
                    servaddr,
                    (jxsocklen_t)addrlen)) {
            fprintf(stderr, "%s sendto() fail, error code: %d.\n", cli->prompt, jxsock_get_last_error());
            return -2;
        }

        if (!JX_MFLAG_WAIT_RECV(mflag)) {
            return 0;
        }

        s_reset_recvtimer();
        if ((recvbytes = s_recvmsg(cli->hsock_sendrecv, cli->recvbuf, sizeof(cli->recvbuf))) == -3) {
            return -3;
        }
    }

    if (s_recv_stat == RECV_TIMEOUT) {
        s_rttinit = 0;
        return -1;
    }

    rtt_stop(&s_rttinfo, rtt_ts(&s_rttinfo) - ((struct jxmsg_header*)cli->recvbuf)->header.ts);
    jxmsg_copyaddr((struct jxmsg_header*)cli->recvbuf, servaddr, addrlen);

    return recvbytes - sizeof(struct jxmsg_header);
}

int s_recvmsg(jxsock_t hsock, void* inmsg, size_t inbytes)
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
            errcode = jxsock_get_last_error();
            if (ETRYAGAIN(errcode)) {
                continue;
            } else {
                fprintf(stderr, "recvfrom() fail, error code: %d.\n", errcode);
                return -3;
            }
        } else if (recvbytes > (ssize_t)sizeof(struct jxmsg_header) &&
            ((struct jxmsg_header*)inmsg)->header.seq == s_sendhdr.seq) {

            s_recv_stat = RECV_OK;
            return recvbytes;
        }

    }
}

void s_reset_recvtimer(void)
{
    jxtimelong_cur(&s_recvbegin);
    s_recvtimer = rtt_start(&s_rttinfo);
}

int s_recv_elapsed(void)
{
    s_recvtimer -= (int)jxtimelong_span_sec(&s_recvbegin);
    return (s_recvtimer <= 0);
}

void s_sendrecv_init(void)
{
    if (s_mutex_inited == 0) {
        s_mutex = jxmutex_create();
        jxclearlist_add(s_sendrecv_clear, NULL);
        s_mutex_inited = 1;
    }
}

void s_sendrecv_clear(void* unused)
{
    (void)unused;
    jxmutex_destroy(&s_mutex);
}
