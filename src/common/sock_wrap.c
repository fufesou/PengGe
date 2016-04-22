/**
 * @file sock_wrap.c
 * @brief
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-20
 * @modified  Sun 2015-12-06 18:20:48 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <ws2tcpip.h>
#include  <windows.h>

#if defined (__MINGW32__)
#include    "common/inet_pton_mingw.h"
#endif

#define BLOCK_RW        0
#define NONBLICK_RW     0
#define SEND_NOSIGNAL   0
#else
#include  <errno.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#define BLOCK_RW        MSG_WAITALL
#define NONBLOCK_RW     MSG_DONTWAIT
#define SEND_NOSIGNAL   MSG_NOSIGNAL
#endif

#include  <stdio.h>
#include  <string.h>
#include  <fcntl.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/list.h"
#include    "common/error.h"
#include    "common/sock_types.h"
#include    "common/sock_wrap.h"
#include    "common/clearlist.h"

int jxsock_envinit()
{
#ifdef WIN32
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2,2), &wsadata) != 0)
    {
         printf("WSAStartup failed with error %d\n", WSAGetLastError());
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
         return -1;
    }
    else
    {
         printf("The dll supports the Winsock version %u.%u!\n", LOBYTE(wsadata.wVersion),
                   HIBYTE(wsadata.wVersion));
         printf("The highest version this dll can support is %u.%u\n",
                   LOBYTE(wsadata.wHighVersion), HIBYTE(wsadata.wHighVersion));
    }

    jxclearlist_add(jxsock_envclear, NULL);
#endif

    return 0;
}

void jxsock_envclear(void* unused)
{
    (void)unused;
#ifdef WIN32
    if(WSACleanup() != 0) {
         printf("WSACleanup() failed! Error code: %d\n", WSAGetLastError());
    }
    else {
         printf("WSACleanup() is OK...\n");
    }
#endif
}

int jxsock_get_last_error(void)
{
#ifdef WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

void jxaddrin_set(struct sockaddr_in* addr_in, const char* ip, int port)
{
    memset(addr_in, 0, sizeof(struct sockaddr_in));
    addr_in->sin_family = AF_INET;
    if (ip) {
        if (jxsock_inet_pton(AF_INET, ip, &addr_in->sin_addr.s_addr) == 0) {
            jxfatal("the ip: \"%s\" is not a valid ipv4 address.\n", ip);
        }
    }
    else {
        addr_in->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    addr_in->sin_port = htons(port);
}

jxsock_t jxsock_open(int tcpudp)
{
    int error;
    int protocol = 0;
    jxsock_t hsock;

    if (tcpudp == SOCK_STREAM) {
        protocol = IPPROTO_TCP;
    }
    else if (tcpudp == SOCK_DGRAM) {
        protocol = IPPROTO_UDP;
    }
    hsock = socket(AF_INET, tcpudp, protocol);
    if (!IS_SOCK_HANDLE(hsock)) {
        error = 1;
        jxfatal_ext(&error, jxerr_exit, "error at socket(), error code: %d\n",  jxsock_get_last_error());
    }

    return hsock;
}

void jxsock_close(jxsock_t handle)
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
        printf("cannot close \"socket\". error code: %d\n", jxsock_get_last_error());
    }
    else {
        printf("closing \"socket\" ...\n");
    }
}

void jxsock_connect(jxsock_t handle, const struct sockaddr* sa, jxsocklen_t addrlen)
{
    jxerr_t error;
    if (connect(handle, sa, addrlen) != 0) {
        jxsock_close(handle);
        error = 1;
        jxfatal_ext(&error, jxerr_exit, "connect error. error code: %d.\n", jxsock_get_last_error());
    }
}

void jxsock_bind(jxsock_t handle, struct sockaddr* sa, jxsocklen_t addrlen)
{
    int error;
    if ((bind(handle, sa, addrlen)) != 0) {
        jxsock_close(handle);
        error = 1;
        jxfatal_ext(&error, jxerr_exit, "error at listen(), error code: %d\n", jxsock_get_last_error());
    }

}

void jxsock_listen(jxsock_t handle, int maxconn)
{
    int error;
    if (listen(handle, maxconn) != 0) {
        jxsock_close(handle);
        error = 1;
        jxfatal_ext(&error, jxerr_exit, "error at listen(), error code: %d\n", jxsock_get_last_error());
    }
}

jxsock_t jxsock_accept(jxsock_t handle, const struct sockaddr* sa, jxsocklen_t* addrlen)
{
    int error;
    jxsock_t hsock;

#ifdef WIN32
    if ((hsock = accept(handle, (struct sockaddr*)sa, addrlen)) == INVALID_SOCKET) {
#else
    if ((hsock = accept(handle, (struct sockaddr*)sa, addrlen)) == -1) {
#endif
        jxsock_close(handle);
        error = 1;
        jxfatal_ext(&error, jxerr_exit, "error at accept(), error code: %d\n", jxsock_get_last_error());
    }
    return hsock;
}

int jxsock_block(jxsock_t handle, int block)
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

int jxsock_print(jxsock_t handle, const char* header)
{
    struct sockaddr_in addr_in;
    jxsocklen_t nlen = sizeof(struct sockaddr_in);
    const char* msgheader = "";
    char addrstr[INET6_ADDRSTRLEN];

    if (!IS_SOCK_HANDLE(handle)) {
        return JX_INVALIE_ARGS;
    }

    msgheader = (header == NULL) ? msgheader : header;

    if (getsockname(handle, (struct sockaddr*)&addr_in, &nlen) == 0) {
        jxsock_inet_ntop(AF_INET, &addr_in.sin_addr, addrstr, sizeof(addrstr));
        fprintf(stdout, "%s IP(s) used: %s\n", msgheader, addrstr);
        fprintf(stdout, "%s port used: %d\n", msgheader, htons(addr_in.sin_port));
        return JX_NO_ERR;
    }
    return JX_WARNING;
}

int jxsock_getsockname(jxsock_t handle, struct sockaddr* addr, jxsocklen_t* addrlen)
{
    int res = 0;
    if ((res = getsockname(handle, addr, addrlen)) != 0) {
        fprintf(stdout, "getsockname() fail, error code: %d.\n", jxsock_get_last_error());
    }
    return res;
}

int jxsock_getpeername(jxsock_t handle, struct sockaddr* addr, jxsocklen_t* addrlen)
{
    int res = 0;
    if ((res = getpeername(handle, addr, addrlen)) != 0) {
        fprintf(stdout, "getpeername() fail, error code: %d.\n", jxsock_get_last_error());
    }
    return res;
}

#ifdef WIN32
const char* jxsock_inet_ntop(int af, const void* src, char* dst, jxsocklen_t size)
{
#ifdef _MSC_VER

#ifndef UNICODE
	return InetNtop(af, (void*)src, dst, size);
#else
	WCHAR wsrc[INET6_ADDRSTRLEN];
	LPWSTR wdst = (LPWSTR)malloc(sizeof(WCHAR) * size);
	swprintf_s(wsrc, sizeof(wsrc) / sizeof(wsrc[0]), L"%hs", (char*)src);
	if (InetNtop(af, (PVOID)wsrc, wdst, size) == NULL) {
		return NULL;
	}
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, wdst, -1, dst, size, NULL, NULL);
	return dst;
#endif

#elif defined(__MINGW32__)
    (void)af; (void)src; (void)dst; (void)size;
    jxfatal("inet ntop not supported for now!\n");
    return 0;
#else
#error unkown compile tool
#endif
}

int jxsock_inet_pton(int af, const char* src, void* dst)
{
#ifdef _MSC_VER

#ifndef UNICODE
	return InetPton(af, src, dst);
#else
	int ret_flag = -1;
	WCHAR wsrc[INET6_ADDRSTRLEN];
	WCHAR wdst[INET6_ADDRSTRLEN];

	swprintf_s(wsrc, sizeof(wsrc) / sizeof(wsrc[0]), L"%hs", src);

	ret_flag = InetPton(af, wsrc, wdst);

	// FIXME: assign 'INET6_ADDRSTRLEN' may be not safe here
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, wdst, -1, dst, INET6_ADDRSTRLEN, NULL, NULL);

	return ret_flag;
#endif

#elif defined(__MINGW32__)
	return inet_pton(af, src, dst);
#else
#error unkown compiler tool
#endif
}
#endif
