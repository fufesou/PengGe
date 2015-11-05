/**
 * @file main.c
 * @brief
 * @author cxl
 * @version 0.1
 * @date 2015-09-25
 * @modified
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#endif

#include  <stdio.h>
#include  <stdlib.h>
#include  <stdint.h>
#include    "sock_types.h"
#include    "sock_wrap.h"
#include    "server.h"


void s_check_args(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    struct csserver udpserver;

    s_check_args(argc, argv);

    cssock_envinit();

    csserver_init(&udpserver, SOCK_DGRAM, atoi(argv[1]), htonl(INADDR_ANY));
    cssock_print(udpserver.hsock, udpserver.msgheader);
    csserver_udp(&udpserver);

    cssock_envclear();

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

