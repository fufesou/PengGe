/**
 * @file client_udp.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 */

#include "client_udp.h"
#include "udp_utility.h"
#include "client_sendrecv.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_loginmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static char* s_cli_msgheader = "client";

static void s_clear(struct client_udp* cli_udp);
static void s_set_socket(struct client_udp* cli_udp);
static void s_print_info(const struct client_udp* cli_udp);
static void s_connect_serv(struct client_udp* cli_udp, const struct sockaddr* serveraddr, int serveraddr_len);
static void s_dg_client(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len);
static void s_login_session(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len);


void check_args(int argc, char* argv[])
{
    if (argc < 2) {
        printf("usage: server <port>.");
        exit(1);
    }
    (void)(argv);
}

void init_client_udp(struct client_udp *cli_udp)
{
    cli_udp->msgheader = s_cli_msgheader;
    cli_udp->set_socket = s_set_socket;
    cli_udp->dg_client = s_dg_client;
    cli_udp->clear = s_clear;
}

void s_print_info(const struct client_udp *cli_udp)
{
    if (U_printf_sockinfo(cli_udp->socket, cli_udp->msgheader) != 0) {
        U_errexit_value(1, "%s: some error occured before communication begin.\n", cli_udp->msgheader);
    }
}

void s_set_socket(struct client_udp *cli_udp)
{
    if (strcmp(cli_udp->msgheader, s_cli_msgheader) != 0) {
        init_client_udp(cli_udp);
    }

    cli_udp->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (cli_udp->socket == INVALID_SOCKET) {
        U_errexit_value(1, "%s: error at socket(), error code: %d\n", cli_udp->msgheader, WSAGetLastError());
    }
    printf("%s: socket() is OK!\n", cli_udp->msgheader);
}

void s_connect_serv(struct client_udp* cli_udp, const struct sockaddr* serveraddr, int serveraddr_len)
{
    if (connect(cli_udp->socket, serveraddr, serveraddr_len) != 0) {
        U_errexit_value(1, "%s: cannot connect server. error code: %d.\n", cli_udp->msgheader, WSAGetLastError());
    } else {
        s_print_info(cli_udp);
    }
}

void s_clear(struct client_udp *cli_udp)
{
    U_close_socket(cli_udp->socket);
}

void s_dg_client(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len)
{
    ssize_t numbytes;

    s_connect_serv(cli_udp, serveraddr, serveraddr_len);

    while (fgets(cli_udp->sendbuf, sizeof(cli_udp->sendbuf), fp) != NULL) {
        cli_udp->sendbuf[strlen(cli_udp->sendbuf) - 1] = '\0';
        numbytes = client_sendrecv(
                    cli_udp->socket,
                    cli_udp->sendbuf,
                    // sizeof(cli_udp->sendbuf),
                    strlen(cli_udp->sendbuf) + 1,
                    cli_udp->recvbuf,
                    sizeof(cli_udp->recvbuf),
                    (struct sockaddr*)serveraddr,
                    serveraddr_len);
        if (numbytes > 0) {
            if (numbytes >= (ssize_t)(sizeof(cli_udp->recvbuf))) {
                fflush(stdout);
                fputs("client: recived msg cut off", stderr);
                fflush(stderr);
            }
            cli_udp->recvbuf[numbytes] = 0;

            if (strncmp(cli_udp->recvbuf, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1)) {
                s_login_session(cli_udp, fp, serveraddr, serveraddr_len);
            } else if (strncmp(cli_udp->recvbuf, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1)) {
                printf("%s: login failed.\n", cli_udp->msgheader);
            } else {
                printf("%s: unkown message.\n", cli_udp->msgheader);
            }
        }
    }
}

void s_login_session(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len)
{
    ssize_t numbytes;

    printf("%s: switch to login interface.\n", cli_udp->msgheader);

    while (fgets(cli_udp->sendbuf, sizeof(cli_udp->sendbuf), fp) != NULL) {
        cli_udp->sendbuf[strlen(cli_udp->sendbuf) - 1] = '\0';
        numbytes = client_sendrecv(
                    cli_udp->socket,
                    cli_udp->sendbuf,
                    // sizeof(cli_udp->sendbuf),
                    strlen(cli_udp->sendbuf) + 1,
                    cli_udp->recvbuf,
                    sizeof(cli_udp->recvbuf),
                    (struct sockaddr*)serveraddr,
                    serveraddr_len);
        if (numbytes > 0) {
            if (numbytes >= (ssize_t)(sizeof(cli_udp->recvbuf))) {
                fflush(stdout);
                fputs("client: recived msg cut off", stderr);
                fflush(stderr);
            }
            cli_udp->recvbuf[numbytes] = 0;
            fputs(cli_udp->recvbuf, stdout);
        }
    }

    printf("%s: exit login interface.\n", cli_udp->msgheader);
}


#ifdef __cplusplus
}
#endif
