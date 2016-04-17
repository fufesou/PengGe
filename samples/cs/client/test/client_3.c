/**
 * @file client_2.c
 * @brief simple client test
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-21
 */

#include <winsock2.h>
#include <stdio.h>
#include "udp_utility.h"


int main(int argc, char* argv[])
{
    WSADATA wsadata;
    SOCKET sending_socket;
    SOCKADDR_IN receiver_addr;
    SOCKADDR_IN srcinfo;
    unsigned int port = 5150;
    char sendbuf[1024] = "sending all my love, sending all my love to youuu!";
    int buflen = 1024;
    int nlen;
    int byte_sent;

    U_init_winsock2(&wsadata);
    printf("client: winsock DLL status is %s.\n", wsadata.szSystemStatus);

    sending_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sending_socket == INVALID_SOCKET) {
        U_errexit_value(-1, "client: socket() failed! error code: %d\n", WSAGetLastError());
    }
    else {
        printf("client: socket() is OK!\n");
    }

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(port);
    receiver_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("client: data to be sent: \"%s\"\n", sendbuf);
    printf("client: sending datagrams...\n");
    byte_sent = sendto(sending_socket, sendbuf, buflen, 0, (SOCKADDR*)&receiver_addr, sizeof(receiver_addr));

    printf("client: sendto() looks OK!\n");

    memset(&srcinfo, 0, sizeof(srcinfo));
    nlen = sizeof(srcinfo);

    if (getsockname(sending_socket, (SOCKADDR*)&srcinfo, &nlen) == 0) {
        printf("client: sending IS(s) used: %s\n", inet_ntoa(srcinfo.sin_addr));
        printf("client: sending port used: %d\n", htons(srcinfo.sin_port));
    } else {
        printf("clinet: error code %d\n", WSAGetLastError());
    }

    nlen = sizeof(receiver_addr);
    getpeername(sending_socket, (SOCKADDR*)&receiver_addr, &nlen);
    printf("client: receiving IP used: %s\n", inet_ntoa(receiver_addr.sin_addr));
    printf("client: receiving port used: %d\n", htons(receiver_addr.sin_port));
    printf("client: total byte sent: %d\n", byte_sent);

    printf("client: finish sending. closing the sending socket...\n");
    U_close_socket(sending_socket);
    U_cleanup_winsock2();

    return 0;
}
