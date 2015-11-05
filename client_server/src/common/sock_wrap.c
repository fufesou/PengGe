/**
 * @file sock_wrap.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 * @modified  Tue 2015-11-03 19:29:30 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#define BLOCK_RW        0
#define NONBLICK_RW     0
#define SEND_NOSIGNAL   0
#define ETRYAGAIN(x) ((x)==WSAEWOULDBLOCK)
#else
#include  <errno.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#define IS_SOCK_HANDLE(x) ((x)>0)
#define BLOCK_RW        MSG_WAITALL
#define NONBLOCK_RW     MSG_DONTWAIT
#define SEND_NOSIGNAL   MSG_NOSIGNAL
#define ETRYAGAIN(x) ((x)==EWOULDBLOCK)
#endif

#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include  <fcntl.h>
#include    "macros.h"
#include    "error.h"
#include    "sock_types.h"
#include    "sock_wrap.h"

int cssock_envinit()
{
#ifdef WIN32
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
    {
         printf("WSAStartup failed with error %d\n", WSAGetLastError());
         // exit(-1);
         return -1;
    }
    else
    {
         printf("The Winsock dll found!\n");
         printf("The current status is %s.\n", wsadata.szSystemStatus);
    }

    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2 )
    {
         printf("The dll do not support the Winsock version %u.%u!\n",
                   LOBYTE(wsadata.wVersion),HIBYTE(wsadata.wVersion));
         WSACleanup();
         // exit(-1);
         return -1;
    }
    else
    {
         printf("The dll supports the Winsock version %u.%u!\n", LOBYTE(wsadata.wVersion),
                   HIBYTE(wsadata.wVersion));
         printf("The highest version this dll can support is %u.%u\n",
                   LOBYTE(wsadata.wHighVersion), HIBYTE(wsadata.wHighVersion));
    }
#endif

    return 0;
}

void cssock_envclear()
{
#ifdef WIN32
    if(WSACleanup() != 0) {
         printf("WSACleanup() failed! Error code: %d\n", WSAGetLastError());
    }
    else {
         printf("WSACleanup() is OK...\n");
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
	int error;
    int protocol = 0;
	cssock_t hsock;

    if (tcpudp == SOCK_STREAM) {
        protocol = IPPROTO_TCP;
    }
    else if (tcpudp == SOCK_DGRAM) {
        protocol = IPPROTO_UDP;
    }
	hsock = socket(AF_INET, tcpudp, protocol); 
    if (!IS_SOCK_HANDLE(hsock)) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "error at socket(), error code: %d\n",  cssock_get_last_error());
    }

    return hsock;
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

void cssock_connect(cssock_t handle, const struct sockaddr* sa, cssocklen_t addrlen)
{
	cserr_t error;
    if (connect(handle, sa, addrlen) != 0) {
		cssock_close(handle);
		error = 1;
        csfatal_ext(&error, cserr_exit, "connect error. error code: %d.\n", cssock_get_last_error());
    }
}

void cssock_bind(cssock_t handle, struct sockaddr* sa, cssocklen_t addrlen)
{
	int error;
	if ((bind(handle, sa, addrlen)) != 0) {
		cssock_close(handle);
		error = 1;
		csfatal_ext(&error, cserr_exit, "error at listen(), error code: %d\n", cssock_get_last_error());
	}
	
}

void cssock_listen(cssock_t handle, int maxconn)
{
	int error;
	if (listen(handle, maxconn) != 0) {
		cssock_close(handle);
		error = 1;
		csfatal_ext(&error, cserr_exit, "error at listen(), error code: %d\n", cssock_get_last_error());
	}
}

cssock_t cssock_accept(cssock_t handle, const struct sockaddr* sa, cssocklen_t* addrlen)
{
	int error;
	cssock_t hsock;

#ifdef WIN32
    if ((hsock = accept(handle, (struct sockaddr*)sa, addrlen)) == INVALID_SOCKET) {
#else
	if ((hsock = accept(handle, (struct sockaddr*)sa, addrlen)) == -1) {
#endif
		cssock_close(handle);
		error = 1;
		csfatal_ext(&error, cserr_exit, "error at accept(), error code: %d\n", cssock_get_last_error());
	}
	return hsock;
}

int cssock_block(cssock_t handle, int block)
{
    unsigned long mode;
	int ctlret;
    if (IS_SOCK_HANDLE(handle))
    {
#ifdef WIN32
        mode = (unsigned long)block;
        if ((ctlret = ioctlsocket(handle, FIONBIO, &mode)) != NO_ERROR) {
			printf("ioctlsocket falied with error: %d\n", ctlret);
			return -1;
		}
		return 0;
#else
        mode = fcntl(handle, F_GETFL, 0);
        mode = block ? (mode | O_NONBLOCK) : (mode & ~O_NONBLOCK);
        if ((ctlret = fcntl(handle, F_SETFL, mode)) != 0) {
			printf("fcntl failed to set blocking mode, error: %d\n", errno);
			return -1;
		}
		return 0;
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

    if (!IS_SOCK_HANDLE(handle)) {
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
