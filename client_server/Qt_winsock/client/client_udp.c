/**
 * @file client_udp.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 */

#include "client_udp.h"
#include "utility.h"

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
static int s_dg_send_recv(struct client_udp* cli_udp, const void* to_addr, int to_addr_len);

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
    cli_udp->dg_send_recv = s_dg_send_recv;
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

int s_dg_send_recv(struct client_udp* cli_udp, const void* to_addr, int to_addr_len)
{

}

#ifdef __cplusplus
}
#endif
