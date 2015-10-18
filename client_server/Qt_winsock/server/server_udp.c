/**
 * @file server_udp.c
 * @brief  The functions prefexed with "s_" are static functions.
 * @author cxl
 * @version 0.1
 * @date 2015-09-28
 */


#include  <stdlib.h>
#include  <stdio.h>
#include  <stdint.h>
#include 	"server_udp.h"
#include    "udp_utility.h"
#include    "udp_types.h"
#include    "server_sendrecv.h"
#include	"server_servroutine.h"


#ifdef __cplusplus
extern "C" {
#endif

static char* s_serv_msgheader = "server";

static void s_create_server(struct server_udp* serv_udp, int af, u_short port, u_long addr);

static void s_print_info(struct server_udp* serv_udp);

static void s_communicate(struct server_udp* serv_udp);

static void s_clear(struct server_udp* serv_udp);


#ifdef __cplusplus
}
#endif


void check_args(int argc, char* argv[])
{
    if (argc < 2) {
        printf("usage: server <port>.");
        exit(1);
    }
    (void)(argv);
}

void init_server_udp(struct server_udp *serv_udp)
{
    serv_udp->msgheader = s_serv_msgheader;
    serv_udp->create_server = s_create_server;
    serv_udp->print_info = s_print_info;
    serv_udp->communicate = s_communicate;
    serv_udp->clear = s_clear;
}

void s_create_server(struct server_udp *serv_udp, int af, u_short port, u_long addr)
{
    if (strcmp(serv_udp->msgheader, s_serv_msgheader) != 0) {
        init_server_udp(serv_udp);
    }

    serv_udp->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (serv_udp->socket == INVALID_SOCKET) {
        U_errexit_value(1, "%s: error at socket(), error code: %d\n", serv_udp->msgheader, WSAGetLastError());
    }
    printf("%s: socket() is OK!\n", serv_udp->msgheader);

    if (port == 0) {
        printf("invalid port!\n");
        closesocket(serv_udp->socket);
    }

    serv_udp->sockaddr_in.sin_family = af;
    serv_udp->sockaddr_in.sin_port = htons(port);
    serv_udp->sockaddr_in.sin_addr.s_addr = addr;

    if (bind(serv_udp->socket, (SOCKADDR*)&serv_udp->sockaddr_in, sizeof(serv_udp->sockaddr_in)) == SOCKET_ERROR) {
        closesocket(serv_udp->socket);
        U_errexit_value(-1, "server: bind() failed! Error code: %d\n", WSAGetLastError());
    }
    else {
        printf("server: bind() is OK\n");
    }
}

void s_print_info(struct server_udp *serv_udp)
{
    if (U_printf_sockinfo(serv_udp->socket, serv_udp->msgheader) != 0) {
        U_errexit_value(1, "%s: some error occured before communication begin.\n", serv_udp->msgheader);
    }
}

void s_communicate(struct server_udp *serv_udp)
{
    process_communication(serv_udp);
}

void s_clear(struct server_udp *serv_udp)
{
    U_close_socket(serv_udp->socket);
}
