/**
 * @file udp_utility.c
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 */

#include <stdio.h>
#include <stdlib.h>
#include "udp_utility.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* g_loginmsg_header = "login: ";
const char* g_logoutmsg_header = "logout";
const char* g_loginmsg_SUCCESS = "login_success";
const char* g_loginmsg_FAIL = "login_fail";
const char g_login_delimiter = ' ';

#ifdef __cplusplus
}
#endif



void U_errexit(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    WSACleanup();
    exit(1);
}

void U_errexit_value(int exit_value, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    WSACleanup();
    exit(exit_value);
}


void U_init_winsock2(WSADATA* wsadata)
{
    if (WSAStartup(MAKEWORD(2,2), wsadata) != 0)
    {
         printf("Server: WSAStartup failed with error %d\n", WSAGetLastError());
         exit(-1);
    }
    else
    {
         printf("Server: The Winsock dll found!\n");
         printf("Server: The current status is %s.\n", wsadata->szSystemStatus);
    }

    if (LOBYTE(wsadata->wVersion) != 2 || HIBYTE(wsadata->wVersion) != 2 )
    {
         printf("Server: The dll do not support the Winsock version %u.%u!\n",
                   LOBYTE(wsadata->wVersion),HIBYTE(wsadata->wVersion));
         WSACleanup();
         exit(-1);
    }
    else
    {
         printf("Server: The dll supports the Winsock version %u.%u!\n", LOBYTE(wsadata->wVersion),
                   HIBYTE(wsadata->wVersion));
         printf("Server: The highest version this dll can support is %u.%u\n",
                   LOBYTE(wsadata->wHighVersion), HIBYTE(wsadata->wHighVersion));
    }
}

void U_cleanup_winsock2()
{
    if(WSACleanup() != 0) {
         printf("Server: WSACleanup() failed! Error code: %d\n", WSAGetLastError());
    }
    else {
         printf("Server: WSACleanup() is OK...\n");
    }
}

void U_close_socket(SOCKET sockfd)
{
   if (closesocket(sockfd) != 0) {
        printf("client: cannot close \"sending_socket\". error code: %d\n", WSAGetLastError());
    }
    else {
        printf("client: closing \"sending_socket\" ...\n");
    }

}

int U_printf_sockinfo(SOCKET sockfd, char *msgheader)
{
    SOCKADDR_IN sockaddr_in;
    int nlen = sizeof(sockaddr_in);
    if (getsockname(sockfd, (SOCKADDR*)&sockaddr_in, &nlen) == 0) {
        printf("%s: receiving IP(s) used: %s\n", msgheader, inet_ntoa(sockaddr_in.sin_addr));
        printf("%s: receiving port used: %d\n", msgheader, htons(sockaddr_in.sin_port));

        return 0;
    }

    return 1;
}
