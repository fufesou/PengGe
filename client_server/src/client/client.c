/**
 * @file client.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 * @modified  Tue 2015-11-03 19:19:00 (+0800)
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

struct cssendrecv_pool;

#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "macros.h"
#include    "sock_types.h"
#include    "error.h"
#include    "client.h"
#include    "sock_wrap.h"
#include    "msgwrap.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_loginmsg_header;
extern const char* g_logoutmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static char* s_cli_msgheader = "client:";

static void s_enter_init_session(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen);
static void s_enter_login_session(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen);

#ifdef __cplusplus
}
#endif


void csclient_init(struct csclient* cli, int tcpudp)
{
	int error;
	int nonblocking = 1;

	cli->msgheader = s_cli_msgheader;

    cli->hsock = cssock_open(tcpudp);
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

void csclient_connect(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
	cssock_connect(cli->hsock, servaddr, addrlen);
	if (csclient_print(cli) != 0) {
		cssock_envclear();
		exit(1);
	}
}

void csclient_communicate(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
	s_enter_init_session(cli, fp, servaddr, addrlen);
}

void s_enter_init_session(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    ssize_t numbytes;
    char* msgdata = NULL;

    csclient_connect(cli, servaddr, addrlen);

    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';
        numbytes = csclient_sendrecv(cli, servaddr, addrlen);
			if (numbytes > 0) {
            if (numbytes >= (ssize_t)(sizeof(cli->recvbuf))) {
                fflush(stdout);
                fprintf(stderr, "%s: recived msg cut off", cli->msgheader);
                fflush(stderr);
            }
            cli->recvbuf[numbytes] = 0;

            msgdata = cli->recvbuf + sizeof(struct csmsg_header);
            if (strncmp(msgdata, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1) == 0) {
                s_enter_login_session(cli, fp, servaddr, addrlen);
            } else if (strncmp(msgdata, g_loginmsg_FAIL, strlen(g_loginmsg_FAIL) + 1) == 0) {
                printf("%s: login failed.\n", cli->msgheader);
            } else {
                printf("%s: unkown message-%s.\n", cli->msgheader, msgdata);
            }
        }
    }
}

void s_enter_login_session(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    ssize_t numbytes;
    char* msgdata = NULL;

    printf("%s: switch to login interface.\n", cli->msgheader);

    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';
        numbytes = csclient_sendrecv(cli, servaddr, addrlen);
        if (numbytes > 0) {
            if (numbytes >= (ssize_t)(sizeof(cli->recvbuf))) {
                fflush(stdout);
                fprintf(stderr, "%s: recived msg cut off", cli->msgheader);
                fflush(stderr);
            }
            cli->recvbuf[numbytes] = 0;

            msgdata = cli->recvbuf + sizeof(struct csmsg_header);
            if (strncmp(msgdata, g_logoutmsg_header, strlen(g_logoutmsg_header) + 1) == 0) {
                printf("%s: exit login interface.\n", cli->msgheader);
                break;
            } else {
                fputs(msgdata, stdout);
                fflush(stdout);
            }
        }
    }
}
