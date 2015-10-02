/**
 * @file server_helper.h
 * @brief This file provide some helper functions for server.
 * @author cxl
 * @version 0.1
 * @date 2015-09-26
 */

#ifndef  SERVER_HELPER_H
#define  SERVER_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <winsock2.h>

struct server_udp
{
    SOCKET socket;
    SOCKADDR_IN sockaddr_in;
    char* msgheader;
    char msgbuf[1024];

    void (*create_socket)(struct server_udp* serv_udp);

    void (*bind_socket)(struct server_udp* serv_udp, int af, u_short port, u_long addr);

    void (*print_info)(struct server_udp* serv_udp);

    void (*communicate)(struct server_udp* serv_udp);

    void (*clear)(struct server_udp* serv_udp);
};

/**
 * @brief check_args simply check whether the program arguments are valid. If the arguments meet the
 * requirements, test_args set the port value, else call exit(1).
 *
 * @param argc
 * @param argv[]
 */
void check_args(int argc, char* argv[]);

void init_server_udp(struct server_udp* serv_udp);


#ifdef __cplusplus
}
#endif

#endif  // SERVER_HELPER_H
