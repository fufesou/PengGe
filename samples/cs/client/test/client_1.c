/**
 * @file client_1.c
 * @brief simple client test
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-21
 */

#include <winsock2.h>
#include <stdio.h>
#include "utility.h"


int main(int argc, char* argv[])
{
    WSADATA wsadata;
    SOCKET sending_socket;
    SOCKADDR_IN serveraddr;
    unsigned int port = 80;
    int retcode;

    init_winsock2(&wsadata);

    printf("client: winsock DLL status is %s.\n", wsadata.szSystemStatus);
    sending_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sending_socket == INVALID_SOCKET) {
        errexit_value(-1, "client: socket() failed! error code: %d\n", WSAGetLastError());
    }
    else {
        printf("client: socket() is OK!\n");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr("192.168.1.102");

    retcode = connect(sending_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retcode != 0) {
        closesocket(sending_socket);
        errexit_value(-1, "client: connect() failed! error code: %d\n", WSAGetLastError());
    }
    else {
        printf("client: connect() is OK, got connected...\n");
        printf("client: ready for sending and receiving data...\n");
    }

    close_socket(sending_socket);
    cleanup_winsock2();

    return 0;
}
