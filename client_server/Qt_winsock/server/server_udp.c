/**
 * @file server_udp.c
 * @brief  The functions prefexed with "s_" are static functions.
 * @author cxl
 * @version 0.1
 * @date 2015-09-28
 */


#include "server_udp.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "udp_utility.h"
#include "udp_types.h"
#include "server_sendrecv.h"


#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_loginmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static char* s_serv_msgheader = "server";


/**
 * @brief s_process_msg is used for handling the message from client, which could be the login information,
 * controlling sequence, or others.
 *
 * @param msg
 * @param len
 */
static void s_process_msg(struct server_udp* serv_udp, int msglen, const struct hdr* hdrdata, const SOCKADDR_IN* to_sockaddr, int sockaddr_len);

/**
 * @TODO Thread should be created here to do this slow work.
 *
 */
static void s_process_login(struct server_udp* serv_udp, int msglen, const struct hdr* hdrdata, const SOCKADDR_IN* to_sockaddr, int sockaddr_len);

static void s_process_communication(struct server_udp* serv_udp, int msglen, const struct hdr* hdrdata, const SOCKADDR_IN* to_sockaddr, int sockaddr_len);

static int s_login_verification(const char* username, const char* password);

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
    int cli_addrlen;
    struct hdr hdrdata;
    ssize_t byte_received;
    SOCKADDR_IN cli_addr;

    printf("%s: I\'m ready to receive a datagram...\n", serv_udp->msgheader);

    while (1) {
        cli_addrlen = sizeof(cli_addr);
        byte_received = server_recv(
                    serv_udp->socket,
                    serv_udp->msgbuf,
                    sizeof(serv_udp->msgbuf),
                    &hdrdata,
                    (struct sockaddr*)&cli_addr,
                    &cli_addrlen);
        if (byte_received > 0) {
            printf("%s: total bytes received: %d\n", serv_udp->msgheader, byte_received);
            printf("%s: the data is \"%s\"\n", serv_udp->msgheader, serv_udp->msgbuf);

            s_process_msg(serv_udp, byte_received, &hdrdata, &cli_addr, cli_addrlen);
        }
        else if (byte_received <= 0) {
            printf("%s: connection closed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
        else {
            printf("%s: recvfrom() failed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
    }

    printf("%s: finish receiving. closing the listening socket...\n", serv_udp->msgheader);
}

void s_clear(struct server_udp *serv_udp)
{
    U_close_socket(serv_udp->socket);
}

void s_process_msg(struct server_udp* serv_udp, int msglen, const struct hdr* hdrdata, const SOCKADDR_IN* to_sockaddr, int sockaddr_len)
{
    if (strncmp(serv_udp->msgbuf, g_loginmsg_header, strlen(g_loginmsg_header)) == 0) {
        s_process_login(serv_udp, msglen, hdrdata, to_sockaddr, sockaddr_len);
    }
    else {
        s_process_communication(serv_udp, msglen, hdrdata, to_sockaddr, sockaddr_len);
    }
}

void s_process_login(struct server_udp *serv_udp, int msglen, const struct hdr* hdrdata, const SOCKADDR_IN *to_sockaddr, int sockaddr_len)
{
    char* username = NULL;
    char* password = NULL;
    char* delimiter = NULL;
    int loginmsg_header_len;

    (void)msglen;

    loginmsg_header_len = strlen(g_loginmsg_header);
    delimiter = strchr(serv_udp->msgbuf + loginmsg_header_len, (int)g_login_delimiter);
    username = serv_udp->msgbuf + loginmsg_header_len;
    *delimiter = '\0';
    password = delimiter + 1;

    getpeername(serv_udp->socket, (SOCKADDR*)to_sockaddr, &sockaddr_len);
    printf("%s: sending IP used: %s\n", serv_udp->msgheader, inet_ntoa(to_sockaddr->sin_addr));
    printf("%s: sending port used: %d\n", serv_udp->msgheader, htons(to_sockaddr->sin_port));

    if (s_login_verification(username, password)) {
        server_send(serv_udp->socket, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1, hdrdata, (SOCKADDR*)to_sockaddr, sockaddr_len);
    }
    else  {
        server_send(serv_udp->socket, g_loginmsg_FAIL, strlen(g_loginmsg_FAIL) + 1, hdrdata, (SOCKADDR*)to_sockaddr, sockaddr_len);
    }
}

int s_login_verification(const char* username, const char* password)
{
    if (strcmp(username, "64bit") == 0) {
        return (strcmp(password, "8-15bytes") == 0);
    }
    return 0;
}

void s_process_communication(struct server_udp *serv_udp, int msglen, const struct hdr* hdrdata, const SOCKADDR_IN *to_sockaddr, int sockaddr_len)
{
    (void)serv_udp;
    (void)msglen;
    (void)hdrdata;
    (void)to_sockaddr;
    (void)sockaddr_len;

    getpeername(serv_udp->socket, (SOCKADDR*)to_sockaddr, &sockaddr_len);
    printf("%s: sending IP used: %s\n", serv_udp->msgheader, inet_ntoa(to_sockaddr->sin_addr));
    printf("%s: sending port used: %d\n", serv_udp->msgheader, htons(to_sockaddr->sin_port));

    server_send(serv_udp->socket, serv_udp->msgbuf, strlen(serv_udp->msgbuf) + 1, hdrdata, (SOCKADDR*)to_sockaddr, sockaddr_len);
}
