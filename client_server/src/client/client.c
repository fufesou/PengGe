/**
 * @file client.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-30
 * @modified  Sat 2015-11-07 13:30:58 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#else
#include  <setjmp.h>
#include  <signal.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#endif

struct csmsgpool;

#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "macros.h"
#include    "sock_types.h"
#include    "error.h"
#include    "client.h"
#include    "sock_wrap.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int g_cli_sendbuflen;
extern int g_cli_recvbuflen;

static char* s_cli_prompt = "client:";


#ifdef __cplusplus
}
#endif


void csclient_init(struct csclient* cli, int tcpudp)
{
	int error;
	int nonblocking = 1;

	cli->loggedin = 0;
    cli->prompt = s_cli_prompt;

    /*
     * @par code to be added
     * @code{.c}
     * cli->len_sendbuf = g_cli_sendbuflen;
     * cli->len_recvbuf = g_cli_recvbuflen;
     * @endcode
     *
     */

    cli->hsock = cssock_open(tcpudp);
    if (cssock_block(cli->hsock, nonblocking) != 0) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s: set socket to non-blocking mode failed, error code: %d\n", cli->prompt, cssock_get_last_error());
	}

    printf("%s: socket() is OK!\n", cli->prompt);
}

void csclient_clear(struct csclient* cli)
{
	(void)cli;

	/*
	 * @par free memory
	 * @code{.c}
	 * if (cli->sendbuf) {
	 *     free(cli->sendbuf);
	 *     cli->sendbuf = 0;
	 * }
	 * if (cli->recvbuf) {
	 *     free(cli->recvbuf);
	 *     cli->recvbuf = 0;
	 * }
	 * @endcode
	 *
	 */
}

int csclient_print(const struct csclient *cli)
{
	int testres;
	cserr_t error;
    if ((testres = cssock_print(cli->hsock, cli->prompt)) == -1) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s socket has not been created.\n", cli->prompt);
    } else if (testres == 1) {
		printf("%s socket has not been connected.\n", cli->prompt);
	}

	return testres;
}

void csclient_connect(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
	cssock_connect(cli->hsock, servaddr, addrlen);
	if (csclient_print(cli) != 0) {
		cssock_envclear();
		exit(1);
	}
}
