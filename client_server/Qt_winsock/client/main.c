/**
 * @file main.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-01
 */


#include <stdio.h>
#include "udp_utility.h"
#include "client_udp.h"


int main(int argc, char* argv[])
{
    WSADATA wsadata;
    struct client_udp udpclient;
    struct sockaddr_in serveraddr;

    check_args(argc, argv);
    U_init_winsock2(&wsadata);

    init_client_udp(&udpclient);
    udpclient.create_client(&udpclient);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons((unsigned short)atoi(argv[1]));
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    udpclient.dg_client(&udpclient, stdin, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    udpclient.clear(&udpclient);
    U_cleanup_winsock2();

    return 0;
}

