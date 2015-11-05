/**
 * @file unix_client_sendrecv.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-31
 * @modified  Fri 2015-11-06 01:11:23 (+0800)
 */

#ifndef WIN32

#include  <stdio.h>
#include  <string.h>
#include  <unistd.h>
#include  <errno.h>
#include  <setjmp.h>
#include  <signal.h>
#include  <sys/socket.h>
#include  <arpa/inet.h>
#include  <pthread.h>
#include  <semaphore.h>
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

static int firstcall = 1;
static sigjmp_buf s_jmpbuf;
static struct csmsg_header s_sendhdr;

static struct rtt_info s_rttinfo;
static int s_rttinit = 0;

/**
 * @brief  s_sig_alarm This function will act as the receive timer.
 *
 * @param signo
 */
static void s_sig_alrm(int signo);
static void s_register_alrm(void);

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

    s_register_alrm();

    ++s_sendhdr.header.seq;
    rtt_newpack(&s_rttinfo);
    cssock_getsockname(cli->hsock, &s_sendhdr.addr, &sendlen);
    s_sendhdr.addrlen = sendlen;

    sendagain:
        s_sendhdr.header.ts = rtt_ts(&s_rttinfo);
        s_sendhdr.numbytes = strlen(cli->sendbuf) + 1;
        csmsg_merge(&s_sendhdr, cli->sendbuf, outbuf, sizeof(outbuf));
        sendto(cli->hsock, outbuf, sizeof(struct csmsg_header) + s_sendhdr.numbytes, 0, servaddr, addrlen);

        alarm(rtt_start(&s_rttinfo));

		if (sigsetjmp(s_jmpbuf, 1) != 0) {
            if (rtt_timeout(&s_rttinfo) < 0) {
                fprintf(stderr, "cssendrecv: no response from server, giving up.\recvbytes");
                s_rttinit = 0;
				errno = ETIMEDOUT;
				return -1;
			}
			goto sendagain;
		}

		do {
            if ((recvbytes = recvfrom(cli->hsock, cli->recvbuf, sizeof(cli->recvbuf), 0, NULL, NULL)) == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf("non-blocking option is set, recve again.\n");
                    continue;
                }
            }
        } while (recvbytes < (ssize_t)sizeof(struct csmsg_header) ||
                    (((struct csmsg_header*)cli->recvbuf)->header.seq != s_sendhdr.header.seq));
		alarm(0);

        rtt_stop(&s_rttinfo, rtt_ts(&s_rttinfo) - ((struct csmsg_header*)cli->recvbuf)->header.ts);

        return recvbytes - sizeof(struct csmsg_header);
}

void s_sig_alrm(int signo)
{
    (void)signo;
    siglongjmp(s_jmpbuf, 1);
}

void s_register_alrm()
{
	if (firstcall) {
		cssignal_ext(SIGALRM, s_sig_alrm);
	}
}

void cssendrecv_init(void)
{
    s_register_alrm();
}

void cssendrecv_clear(void)
{

}

#endif
