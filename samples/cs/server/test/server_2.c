/**
 * @file server_2.c
 * @brief 
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-21
 */


#include <winsock2.h>
#include <stdio.h>
#include "utility.h"


int recvTimeoutTCP(SOCKET sockfd, long sec, long usec);

int main(int argc, char* argv[])
{
    WSADATA wsadata;
    SOCKET listening_socket;
    SOCKET connection_socket;
    SOCKADDR_IN server_addr;
    SOCKADDR_IN sender_info;
    int port = 7171;
    char recvbuff[1024];
    int byte_received;
    int i;
    int nlen;
    int select_timing;

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

    /* printf("server: accept() is ready...\n"); */

    select_timing = recvTimeoutTCP(listening_socket, 10, 10);

    switch (select_timing) {
        case 0:
            printf("\nserver: Timeout lor while waiting you retard client!...\n");
            break;
        case -1:
            printf("\nserver: some error encountered with code number: %d\n", WSAGetLastError());
            break;
        default:
        {
            while (1) {
                connection_socket = SOCKET_ERROR;
                while (connection_socket == (SOCKET)SOCKET_ERROR) {
                    connection_socket = accept(listening_socket, NULL, NULL);
                    printf("server: accept() is OK...\n");
                    printf("server: client connected, ready for receiving and sending data...\n");

                    byte_received = recv(connection_socket, recvbuff, sizeof(recvbuff), 0);
                    if (byte_received > 0) {
                        printf("server: recv() looks find...\n");
                        getsockname(listening_socket, (SOCKADDR*)&server_addr, (int*)sizeof(server_addr));
                        printf("server: receiving IP(s) used: %s\n", inet_ntoa(server_addr.sin_addr));
                        printf("server: receiving port used: %d\n", htons(server_addr.sin_port));

                        memset(&sender_info, 0, sizeof(sender_info));
                        nlen = sizeof(sender_info);

                        getpeername(connection_socket, (SOCKADDR*)&sender_info, &nlen);
                        printf("server: sending IP used: %s\n", inet_ntoa(sender_info.sin_addr));
                        printf("server: sending port used: %d\n", htons(sender_info.sin_port));

                        printf("server: bytes received: %d\n", byte_received);
                        printf("server: those bytes are: \"");
                        for (i=0; i<byte_received; ++i) {
                            printf("%c", recvbuff[i]);
                        }
                        printf("\"");
                    }
                    else if (byte_received == 0) {
                        printf("server: connection closed!\n");
                    }
                    else {
                        printf("server: recv() failed with error code: %d\n", WSAGetLastError());
                    }
                }

                if (shutdown(connection_socket, SD_SEND) != 0) {
                    printf("\nserver: well, there is something wrong with the shutdown(). the error code: %d\n", WSAGetLastError());
                }
                else {
                    printf("\nserver: shutdown() looks OK...\n");
                }

                if (recvTimeoutTCP(listening_socket, 15, 0) == 0) {
                    break;
                }
            }
        }
    }

    printf("\nserver: the listening socket is timeout...\n");

    close_socket(listening_socket);
    cleanup_winsock2();

    return 0;
}

int recvTimeoutTCP(SOCKET sockfd, long sec, long usec)
{
    struct timeval timeout;
    struct fd_set fds;

    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    FD_ZERO(&fds);
    FD_SET(sockfd, &fds);

    return select(1, &fds, 0, 0, &timeout);
}
