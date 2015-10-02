/**
 * @file client.c
 * @brief this client procedure is portable to windows and linux
 * @author cxl
 * @version 0.1
 * @date 2015-09-17
 */

#define MINGW32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MINGW32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#define MAXLINE 1024

int main(int argc, char* argv[])
{
    int sockfd, n;
    char receline[ MAXLINE+1 ];
    struct sockaddr_in serveraddr;

    if (argc != 2)
    {
        printf("Usage: %s IP_address\n", argv[0]);
        exit(0);
    }

#ifdef MINGW32
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(1, 1), &wsadata) == SOCKET_ERROR)
    {
        printf("WSAStartup() fail\n");
        exit(0);
    }
#endif

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("socket() fail\n");
        exit(0);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(1024);

    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr))  == -1)
    {
        printf("connect() fail\n");
        exit(0);
    }

#ifdef MINGW32
    while ((n = recv(sockfd, receline, MAXLINE, 0)) > 0)
#else
    while ((n = read(sockfd, receline, MAXLINE)) > 0)
#endif
    {
        receline[n] = 0;
        if (fputs(receline, stdout) == EOF)
        {
            printf("fputs() error\n");
        }
    }

    if (n < 0)
    {
        printf("read() fail\n");
    }

#ifdef MINGW32
    closesocket(sockfd);
    WSACleanup();
#endif

    return 0;
}
