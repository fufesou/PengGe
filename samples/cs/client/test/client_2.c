/**
 * @file client_2.c
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
    SOCKADDR_IN sender_info;
    unsigned int port = 7171;
    int retcode;
    char sendbuf[1024] = "This is a test string from sender.";
    int byte_sent;
    int nlen;

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
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retcode = connect(sending_socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retcode != 0) {
        closesocket(sending_socket);
        errexit_value(-1, "client: connect() failed! error code: %d\n", WSAGetLastError());
    }
    else {
        printf("client: connect() is OK, got connected...\n");
        printf("client: ready for sending and receiving data...\n");
    }

    nlen = sizeof(serveraddr);
    getsockname(sending_socket, (SOCKADDR*)&serveraddr, &nlen);
    printf("client: receiver IP(s) used: %s\n", inet_ntoa(serveraddr.sin_addr));
    printf("client: receiver port used: %d\n", htons(serveraddr.sin_port));

    byte_sent = send(sending_socket, sendbuf, strlen(sendbuf), 0);

    if (byte_sent == SOCKET_ERROR) {
        printf("client: send() error %d.\n", WSAGetLastError());
    }
    else {
        printf("client: send() is OK -bytes sent: %d\n", byte_sent);
        memset(&sender_info, 0, sizeof(sender_info));
        nlen = sizeof(sender_info);

        getsockname(sending_socket, (SOCKADDR*)&sending_socket, &nlen);
        printf("client: receiver IP(s) used: %s\n", inet_ntoa(serveraddr.sin_addr));
        printf("client: receiver port used: %d\n", htons(serveraddr.sin_port));
        printf("client: those bytes represent: \"%s\"\n", sendbuf);
    }

    if (shutdown(sending_socket, SD_SEND) != 0) {
        printf("client: well, there is something wrong with the shutdown(). The error code: %d\n", WSAGetLastError());
    }
    else {
        printf("client: shutdown() looks OK...\n");
    }

    close_socket(sending_socket);
    cleanup_winsock2();

    return 0;
}
