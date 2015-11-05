/**
 * @file main.c
 * @brief Client must login before exchange message with server. 
 * The format of login message is "login: <username> <password>".
 * The format of logout message is "logout".
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-01
 * @modified  Tue 2015-11-03 18:47:10 (+0800)
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
#include    "client.h"

int main(int argc, char* argv[])
{
    struct csclient udpclient;
    struct sockaddr_in serveraddr;

    if (argc < 2) {
        printf("usage: client <port>.");
        exit(1);
    }

	cssock_envinit();

    csclient_init(&udpclient, SOCK_DGRAM);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons((unsigned short)atoi(argv[1]));
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    csclient_communicate(&udpclient, stdin, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

	cssock_envclear();

    return 0;
}

