/**
 * @file sock_wrap.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 * @modified  Sat 2015-10-31 11:54:06 (+0800)
 */

#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include  <fcntl.h>
#include    "sock_wrap.h"
#include    "sock_types.h"
#include    "macros.h"

#ifdef WIN32
#include  <windows.h>
#include  <winsock2.h>
#define IS_SOCK_HANDLE(x) ((x)!=INVALID_SOCKET)
#define BLOCK_RW        0
#define NONBLICK_RW     0
#define SEND_NOSIGNAL   0
#define ETRYAGAIN(x) ((x)==WSAEWOULDBLOCK)
#else
#include  <errno.h>
#include  <unistd.h>
#include  <arpa/inet.h>
#define IS_SOCK_HANDLE(x) ((x)>0)
#define BLOCK_RW        MSG_WAITALL
#define NONBLOCK_RW     MSG_DONTWAIT
#define SEND_NOSIGNAL   MSG_NOSIGNAL
#define ETRYAGAIN(x) ((x)==EWOULDBLOCK)
#endif


int cssock_envinit()
{
#ifdef WIN32
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
    {
         printf("Server: WSAStartup failed with error %d\n", WSAGetLastError());
         // exit(-1);
         return -1;
    }
    else
    {
         printf("Server: The Winsock dll found!\n");
         printf("Server: The current status is %s.\n", wsadata.szSystemStatus);
    }

    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2 )
    {
         printf("Server: The dll do not support the Winsock version %u.%u!\n",
                   LOBYTE(wsadata.wVersion),HIBYTE(wsadata.wVersion));
         WSACleanup();
         // exit(-1);
         return -1;
    }
    else
    {
         printf("Server: The dll supports the Winsock version %u.%u!\n", LOBYTE(wsadata.wVersion),
                   HIBYTE(wsadata.wVersion));
         printf("Server: The highest version this dll can support is %u.%u\n",
                   LOBYTE(wsadata.wHighVersion), HIBYTE(wsadata.wHighVersion));
    }
#endif

    return 0;
}

void cssock_envclear()
{
#ifdef WIN32
    if(WSACleanup() != 0) {
         printf("Server: WSACleanup() failed! Error code: %d\n", WSAGetLastError());
    }
    else {
         printf("Server: WSACleanup() is OK...\n");
    }
#endif
}

int cssock_get_last_error(void)
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

void csaddrin_set(struct sockaddr_in* addr_in, const char* ip, int port)
{
    memset(addr_in, 0, sizeof(struct sockaddr_in));
    addr_in->sin_family = AF_INET;
    if (ip) {
        addr_in->sin_addr.s_addr = inet_addr(ip);
    }
    else {
        addr_in->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    addr_in->sin_port = htons(port);
}

cssock_t cssock_open(int tcpudp)
{
    int protocol = 0;

    if (tcpudp == SOCK_STREAM) {
        protocol = IPPROTO_TCP;
    }
    else if (tcpudp == SOCK_DGRAM) {
        protocol = IPPROTO_UDP;
    }
    return socket(AF_INET, tcpudp, protocol);
}

void cssock_close(cssock_t handle)
{
    if (!IS_SOCK_HANDLE(handle))
    {
        return;
    }

#ifdef WIN32
    if (closesocket(handle) != 0) {
#else
    if (close(handle) != 0) {
#endif
        printf("cannot close \"sending_socket\". error code: %d\n", cssock_get_last_error());
    }
    else {
        printf("closing \"sending_socket\" ...\n");
    }
}

int cssock_block(cssock_t handle, int block)
{
    unsigned long mode;
    if (IS_SOCK_HANDLE(handle))
    {
#ifdef WIN32
        mode = (unsigned long)block;
        return ioctlsocket(handle, FIONBIO, &mode);
#else
        mode = fcntl(handle, F_GETFL, 0);
        mode = block ? (mode & ~O_NONBLOCK) : (mode | O_NONBLOCK);
        return fcntl(handle, F_SETFL, mode);
#endif
    }
    return -1;
}

int cssock_print(cssock_t handle, const char* header)
{
    struct sockaddr_in addr_in;
#ifdef WIN32
    int nlen;
#else
    socklen_t nlen;
#endif
    nlen = sizeof(struct sockaddr_in);
    const char* msgheader = "";

    if (IS_SOCK_HANDLE(handle)) {
        return -1;
    }

	msgheader = (header == NULL) ? msgheader : header;

    if (getsockname(handle, (struct sockaddr*)&addr_in, &nlen) == 0) {
        fprintf(stdout, "%s IP(s) used: %s\n", msgheader, inet_ntoa(addr_in.sin_addr));
        fprintf(stdout, "%s port used: %d\n", msgheader, htons(addr_in.sin_port));
        return 0;
    }
    return 1;
}
