/**
 * @file main.c
 * @brief
 * @author cxl
 * @version 0.1
 * @date 2015-09-25
 */


#include <winsock2.h>
#include <stdio.h>
#include "udp_utility.h"
#include "server_udp.h"


int main(int argc, char* argv[])
{
    WSADATA wsadata;
    struct server_udp udpserver;

    check_args(argc, argv);
    U_init_winsock2(&wsadata);

    init_server_udp(&udpserver);

    udpserver.create_server(&udpserver, AF_INET, atoi(argv[1]), htonl(INADDR_ANY));
    udpserver.print_info(&udpserver);

    udpserver.communicate(&udpserver);

    udpserver.clear(&udpserver);

    U_cleanup_winsock2();

    return 0;
}
