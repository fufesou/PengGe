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

#define LISTENDQ 10


int main(int argc, char* argv[])
{
    int serverfd, connectfd;
    struct sockaddr_in serveraddr;
    char buff[1024];
    time_t tlick;
    int iRet;

#ifdef MINGW32
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(1, 1), &wsadata) == SOCKET_ERROR)
    {
        printf("WSAStartup() fail\n");
        exit(0);
    }
#endif

    printf("socket()\n");
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1)
    {
        printf("socket() fail\n");
        exit(0);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(1024);

    printf("bind()\n");
    iRet = bind(serverfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (iRet == -1)
    {
        printf("bind() fail\n");
        exit(0);
    }

    printf("listen()\n");
    iRet = listen(serverfd, LISTENDQ);
    if (iRet == -1)
    {
        printf("listen() fail\n");
        exit(0);
    }

    for (;;)
    {
        printf("Waiting for connect...\n");
        connectfd = accept(serverfd, (struct sockaddr*)NULL, NULL);
        tlick = time(NULL);
        snprintf(buff, sizeof(buff), "From server:\n%s", ctime(&tlick));

#ifdef MINGW32
        send(connectfd, buff, strlen(buff), 0);
        closesocket(connectfd);
#else
        write(connectfd, buff, strlen(buff));
        close(connectfd);
#endif

    }

#ifdef MINGW32
        closesocket(serverfd);
        WSACleanup();
#endif

    return 0;
}
