/**
 * @file main1.c
 * @brief 
 * @author cxl
 * @version 1.0
 * @date 2015-09-20
 */

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

int main(int argc, char* argv[])
{
    WSADATA wsadata;
    int ret;

    if ((ret = WSAStartup(MAKEWORD(2, 2), &wsadata)) != 0) {        /* initialize Winsock version 2.2 */
        printf("WSAStartup failed with error %d\n", WSAGetLastError());
        return 1;
    }
    else {
        printf("The Winsock dll found!\n");
        printf("The current status is : %s.\n", wsadata.szSystemStatus);
    }

    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
        printf("The dll do not support the Winsock version %u.%u\n",
                LOBYTE(wsadata.wVersion), HIBYTE(wsadata.wVersion));
        WSACleanup();
        return 0;
    }
    else {
        printf("The dll supports the Winsock version %u.%u!\n", LOBYTE(wsadata.wVersion), HIBYTE(wsadata.wVersion));
        printf("The highest version this dll can support: %u.%u\n", LOBYTE(wsadata.wHighVersion), HIBYTE(wsadata.wHighVersion));

        /* Setup Winsock communication code here */

        if (WSACleanup() == SOCKET_ERROR) {
            printf("WSACleanup failed with error %d\n", WSAGetLastError());
            return 1;
        }
    }

    return 0;
}
