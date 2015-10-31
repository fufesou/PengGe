/**
 * @file client.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 * @modified  Sat 2015-10-31 15:59:00 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#else
#include  <setjmp.h>
#include  <signal.h>
#endif

#include  <stdio.h>
#include  <string.h>
#include    "unprtt.h"
#include    "sock_types.h"
#include    "csclient.h"
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

static void s_enter_init_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len);
static void s_enter_login_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len);

#ifdef __cplusplus
}
#endif


int csclient_print(const struct csclient *cli)
{
	int testres;
	cserr_t error;
	if ((testres = cssock_print(cli->socket, cli->msgheader)) == -1) {
		error = 1;
        csfatal_ext(&error, "%s socket has not been created.\n", cli->msgheader);
    } else if (testres == 1) {
		printf("%s socket has not been connected.\n", cli->msgheader);
	}

	return testres;
}

void s_set_socket(struct csclient *cli)
{
    if (strcmp(cli->msgheader, s_cli_msgheader) != 0) {
        init_client_udp(cli);
    }

    cli->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cli->socket == INVALID_SOCKET) {
        U_errexit_value(1, "%s: error at socket(), error code: %d\n", cli->msgheader, WSAGetLastError());
    }
    printf("%s: socket() is OK!\n", cli->msgheader);
}

void csclient_connect(struct csclient* cli, const struct sockaddr* serveraddr, int serveraddr_len)
{
	cserr_t error;
    if ((res = connect(cli->socket, serveraddr, serveraddr_len)) != 0) {
		error = 1;
        csfatal_ext(&error, "%s cannot connect server. error code: %d.\n", cli->msgheader, cssock_get_last_error());
    } else {
        if (csclient_print(cli) != 0) {
			cssock_envclear();
			exit(1);
		}
    }
}

void csclient_communicate(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len)
{
	s_enter_init_session(cli, fp, serveraddr, serveraddr_len);
}

void s_enter_init_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len)
{
    ssize_t numbytes;

    s_connect_serv(cli, serveraddr, serveraddr_len);

    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';
        numbytes = csclient_sendrecv(cli, serveraddr, serveraddr_len);
			if (numbytes > 0) {
            if (numbytes >= (ssize_t)(sizeof(cli->recvbuf))) {
                fflush(stdout);
                fprintf(stderr, "client: recived msg cut off", cli->msgheader);
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

void s_enter_login_session(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len)
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

int csclient_sendrecv(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len)
{
}

