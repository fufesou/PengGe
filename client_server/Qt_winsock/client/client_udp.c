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

static void s_create_client(struct client_udp* cli_udp);
static void s_print_info(struct client_udp* cli_udp);
static void s_clear(struct client_udp* cli_udp);
static void s_dg_client(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len);


#ifdef WIN32
// http://www.intervalzero.com/library/RTX/WebHelp/Content/PROJECTS/SDK%20Reference/WinsockRef/WSASendTo.htm
#endif


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
    cli_udp->create_client = s_create_client;
    cli_udp->print_info = s_print_info;
    cli_udp->dg_client = s_dg_client;
    cli_udp->clear = s_clear;
}

void s_print_info(struct client_udp *cli_udp)
{
    if (U_printf_sockinfo(cli_udp->socket, cli_udp->msgheader) != 0) {
        U_errexit_value(1, "%s: some error occured before communication begin.\n", cli_udp->msgheader);
    }
}

void s_create_client(struct client_udp *cli_udp)
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

void s_clear(struct client_udp *cli_udp)
{
    U_close_socket(cli_udp->socket);
}

void s_dg_client(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len)
{
    ssize_t numbytes;
    while (fgets(cli_udp->sendbuf, sizeof(cli_udp->sendbuf), fp) != NULL) {
        numbytes = U_send_recv(
                    cli_udp->socket,
                    cli_udp->sendbuf,
                    sizeof(cli_udp->sendbuf),
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
}

#ifdef __cplusplus
}
#endif
