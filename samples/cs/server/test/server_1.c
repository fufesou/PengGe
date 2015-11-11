/**
 * @file server_1.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-20
 */


#include <winsock2.h>
#include <stdio.h>
#include "utility.h"


int main(int argc, char* argv[])
{
    WSADATA wsadata;
    SOCKET listening_socket;
    SOCKET connection_socket;
    SOCKADDR_IN server_addr;
    int port = 5150;

    init_winsock2(&wsadata);

    listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listening_socket == INVALID_SOCKET) {
        errexit_value(-1, "server: error at socket(), error code: %d\n", WSAGetLastError());
    }
    else {
        printf("server socket() is OK!\n");
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listening_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(listening_socket);
        errexit_value(-1, "server: bind() failed! Error code: %d\n", WSAGetLastError());
    }
    else {
        printf("server: bind() is OK\n");
    }

    if (listen(listening_socket, 5) == SOCKET_ERROR) {
        closesocket(listening_socket);
        errexit_value(-1, "server: error listening on the socket %d.\n", WSAGetLastError());
    }
    else {
        printf("server: listen() is OK, I'm waiting for connecting...\n");
    }

    printf("server: accept() is ready...\n");

    while (1) {
        connection_socket = SOCKET_ERROR;
        while (connection_socket == (SOCKET)SOCKET_ERROR) {
            connection_socket = accept(listening_socket, NULL, NULL);
        }

        printf("server: accept() is OK...\n");
        printf("server: client connected, ready for receiving and sending data...\n");
        listening_socket = connection_socket;
        break;
    }

    if (closesocket(connection_socket) != 0) {
        printf("server: cannot close \"connection_socket\". error code: %d\n", WSAGetLastError());
    }
    else {
        printf("server: closing \"connection_socket\" ...\n");
    }

    cleanup_winsock2();

    return 0;
}
