/**
 * @file main.c
 * @brief
 * @author cxl
 * @version 0.1
 * @date 2015-09-25
 */


#include <winsock2.h>
#include <stdio.h>
#include "sock_wrap.h"
#include "server.h"


void s_check_args(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    struct server_udp udpserver;

    s_check_args(argc, argv);

    init_server_udp(&udpserver);

    udpserver.create_server(&udpserver, AF_INET, atoi(argv[1]), htonl(INADDR_ANY));
    udpserver.print_info(&udpserver);

    udpserver.communicate(&udpserver);

    udpserver.clear(&udpserver);

    U_cleanup_winsock2();

    return 0;
}

void s_check_args(int argc, char* argv[])
{
    if (argc < 2) {
        printf("usage: server <port>.");
        exit(1);
    }
    (void)(argv);
}

