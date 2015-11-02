/**
 * @file unix_client_sendrecv.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-31
 * @modified  Mon 2015-11-02 19:29:57 (+0800)
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
#include    "bufarray.h"
#include    "sendrecv_pool.h"
#include    "msgwrap.h"
#include    "utility_wrap.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define RTT_DEBUG

static int firstcall = 1;
static sigjmp_buf s_jmpbuf;
static struct csmsg_header s_sendhdr, s_recvhdr;

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

ssize_t cssendrecv(
			cssock_t hsock, __inout struct rtt_info* rttinfo,
			const void* outmsg, size_t outbytes,
			void* inmsg, size_t inbytes,
			const struct sockaddr* destaddr, cssocklen_t destlen)
{
	char outbuf[MAX_BUF_LEN];
	char inbuf[MAX_BUF_LEN];
    struct sockaddr fromaddr;
    cssocklen_t fromlen = sizeof(fromaddr);
	ssize_t n;

	s_register_alrm();

	sendagain:
		s_sendhdr.header.ts = rtt_ts(rttinfo);
		s_sendhdr.numbytes = outbytes;
		csmsg_merge(&s_sendhdr, outmsg, outbuf, sizeof(outbuf));
        sendto(hsock, outbuf, sizeof(struct csmsg_header) + outbytes, 0, destaddr, destlen);

		alarm(rtt_start(rttinfo));

		if (sigsetjmp(s_jmpbuf, 1) != 0) {
            if (rtt_timeout(rttinfo) < 0) {
				fprintf(stderr, "cssendrecv: no response from server, giving up.\n");
				errno = ETIMEDOUT;
				return -1;
			}
			goto sendagain;
		}

		do {
            if ((n = recvfrom(hsock, inbuf, sizeof(inbuf), 0, &fromaddr, &fromlen)) == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf("non-blocking option is set, recve again.\n");
                    continue;
                }
            }
        } while (n < (ssize_t)sizeof(struct csmsg_header) ||
                    (((struct csmsg_header*)inbuf)->header.seg != s_sendhdr.header.seg));
		alarm(0);

        csmsg_extract_copy(inbuf, &s_recvhdr, inmsg, inbytes);
        rtt_stop(rttinfo, rtt_ts(rttinfo) - s_recvhdr.header.ts);

        return n - sizeof(struct csmsg_header);
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

#endif
