/**
 * @file client_udp.h
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 */

#ifndef  CLIENT_UDP_H
#define  CLIENT_UDP_H

#include <stdio.h>
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

struct client_udp
{
    SOCKET socket;
    SOCKADDR_IN sockaddr_in;
    char* msgheader;
    char sendbuf[1024];
    char recvbuf[1024];

    /**
     * @brief  only AF_INET is supported by now
     *
     * @param client_udp
     */
    void (*create_client)(struct client_udp* cli_udp);
    void (*dg_client)(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len);
    void (*clear)(struct client_udp* cli_udp);
};

void check_args(int argc, char* argv[]);

void init_client_udp(struct client_udp *cli_udp);


#ifdef __cplusplus
}
#endif


#endif  // CLIENT_UDP_H

