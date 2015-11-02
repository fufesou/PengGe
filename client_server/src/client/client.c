/**
 * @file client.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 * @modified  Mon 2015-11-02 19:00:59 (+0800)
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

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "macros.h"
#include    "unprtt.h"
#include    "sock_types.h"
#include    "error.h"
#include    "client.h"
#include    "sock_wrap.h"
#include    "client_sendrecv.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_loginmsg_header;
extern const char* g_logoutmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static char* s_cli_msgheader = "client:";
static struct rtt_info s_rttinfo;
static int s_rttinit = 0;

static void s_enter_init_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len);
static void s_enter_login_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len);

#ifdef __cplusplus
}
#endif


void csclient_init(struct csclient* cli, int tcpudp)
{
	int error;
	int nonblocking = 1;

	cli->msgheader = s_cli_msgheader;

    cli->hsock = cssock_open(tcpudp);
    if (cli->hsock == INVALID_SOCKET) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s: error at socket(), error code: %d\n", cli->msgheader, cssock_get_last_error());
    }
    if (cssock_block(cli->hsock, nonblocking) != 0) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s: set socket to non-blocking mode failed, error code: %d\n", cli->msgheader, cssock_get_last_error());
	}

    printf("%s: socket() is OK!\n", cli->msgheader);
}

int csclient_print(const struct csclient *cli)
{
	int testres;
	cserr_t error;
    if ((testres = cssock_print(cli->hsock, cli->msgheader)) == -1) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s socket has not been created.\n", cli->msgheader);
    } else if (testres == 1) {
		printf("%s socket has not been connected.\n", cli->msgheader);
	}

	return testres;
}

void csclient_connect(struct csclient* cli, const struct sockaddr* serveraddr, int serveraddr_len)
{
	cserr_t error;
    if (connect(cli->hsock, serveraddr, serveraddr_len) != 0) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s cannot connect server. error code: %d.\n", cli->msgheader, cssock_get_last_error());
    } else {
        if (csclient_print(cli) != 0) {
			cssock_envclear();
			exit(1);
		}
    }
}

void csclient_communicate(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len)
{
	s_enter_init_session(cli, fp, serveraddr, serveraddr_len);
}

void s_enter_init_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len)
{
    ssize_t numbytes;

    csclient_connect(cli, serveraddr, serveraddr_len);

    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';
        numbytes = csclient_sendrecv(cli, serveraddr, serveraddr_len);
			if (numbytes > 0) {
            if (numbytes >= (ssize_t)(sizeof(cli->recvbuf))) {
                fflush(stdout);
                fprintf(stderr, "%s: recived msg cut off", cli->msgheader);
                fflush(stderr);
            }
            cli->recvbuf[numbytes] = 0;

            if (strncmp(cli->recvbuf, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1) == 0) {
                s_enter_login_session(cli, fp, serveraddr, serveraddr_len);
            } else if (strncmp(cli->recvbuf, g_loginmsg_FAIL, strlen(g_loginmsg_FAIL) + 1) == 0) {
                printf("%s: login failed.\n", cli->msgheader);
            } else {
                printf("%s: unkown message.\n", cli->msgheader);
            }
        }
    }
}

void s_enter_login_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len)
{
    ssize_t numbytes;

    printf("%s: switch to login interface.\n", cli->msgheader);

    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';
        numbytes = csclient_sendrecv(cli, serveraddr, serveraddr_len);
        if (numbytes > 0) {
            if (numbytes >= (ssize_t)(sizeof(cli->recvbuf))) {
                fflush(stdout);
                fprintf(stderr, "%s: recived msg cut off", cli->msgheader);
                fflush(stderr);
            }
            cli->recvbuf[numbytes] = 0;

            if (strncmp(cli->recvbuf, g_logoutmsg_header, strlen(g_logoutmsg_header) + 1) == 0) {
                printf("%s: exit login interface.\n", cli->msgheader);
                break;
            } else {
                fputs(cli->recvbuf, stdout);
            }
        }
    }
}

ssize_t csclient_sendrecv(struct csclient* cli, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len)
{
	if (s_rttinit == 0) {
		rtt_init(&s_rttinfo);
		s_rttinit = 1;
#ifdef _DEBUG
		rtt_d_flag = 1;
#endif
	}

	return cssendrecv(
				cli->hsock, &s_rttinfo,
				cli->sendbuf, strlen(cli->sendbuf) + 1,
				cli->recvbuf, sizeof(cli->recvbuf),
				serveraddr, serveraddr_len);
}
