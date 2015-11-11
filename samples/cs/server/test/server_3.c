/**
 * @file server_2.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-21
 */


#include <winsock2.h>
#include <stdio.h>
#include "utility.h"


int main(int argc, char* argv[])
{
    WSADATA wsadata;
    SOCKET receiving_socket;
    SOCKADDR_IN receiver_addr;
    SOCKADDR_IN sender_addr;
    int nlen;
    int port = 5150;
    char recvbuff[1024];
    int buf_len = 1024;
    int senderaddr_size = sizeof(sender_addr);
    int byte_received = 5;

    init_winsock2(&wsadata);

    receiving_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (receiving_socket == INVALID_SOCKET) {
        errexit_value(-1, "server: error at socket(), error code: %d\n", WSAGetLastError());
    }
    else {
        printf("server socket() is OK!\n");
    }

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(port);
    receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(receiving_socket, (SOCKADDR*)&receiver_addr, sizeof(receiver_addr)) == SOCKET_ERROR) {
        closesocket(receiving_socket);
        errexit_value(-1, "server: bind() failed! Error code: %d\n", WSAGetLastError());
    }
    else {
        printf("server: bind() is OK\n");
    }

    nlen = sizeof(receiver_addr);
    getsockname(receiving_socket, (SOCKADDR*)&receiver_addr, &nlen);
    printf("server: receiving IP(s) used: %s\n", inet_ntoa(receiver_addr.sin_addr));
    printf("server: receiving port used: %d\n", htons(receiver_addr.sin_port));

    printf("server: I\'m ready to receive a datagram...\n");

    byte_received = recvfrom(receiving_socket, recvbuff, buf_len, 0, (SOCKADDR*)&sender_addr, &senderaddr_size);
    if (byte_received > 0) {
        printf("server: total bytes received: %d\n", byte_received);
        printf("server: the data is \"%s\"\n", recvbuff);
    }
    else if (byte_received <= 0) {
        printf("server: connection closed with error code: %d\n", WSAGetLastError());
    }
    else {
        printf("server: recvfrom() failed with error code: %d\n", WSAGetLastError());
    }

    getpeername(receiving_socket, (SOCKADDR*)&sender_addr, &senderaddr_size);
    printf("server: sending IP used: %s\n", inet_ntoa(sender_addr.sin_addr));
    printf("server: sending port used: %d\n", htons(sender_addr.sin_port));

    printf("server: finish receiving. closing the listening socket...\n");
    close_socket(receiving_socket);
    cleanup_winsock2();

    return 0;
}