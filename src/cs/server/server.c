/**
 * @file server.c
 * @brief  The functions prefexed with "s_" are static functions.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-28
 * @modified  Sat 2015-11-07 11:50:02 (+0800)
 */

#ifdef WIN32
#include  <ws2tcpip.h>
#include  <winsock2.h>
#include  <windows.h>
#else
#include  <setjmp.h>
#include  <signal.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#endif

#include  <stdlib.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <semaphore.h>
#include    "error.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "sock_wrap.h"
#include    "msgpool.h"
#include    "msgwrap.h"
#include 	"server.h"


#ifdef __cplusplus
extern "C" {
#endif

static char* s_serv_prompt = "server:";

#ifdef __cplusplus
}
#endif


void csserver_init(struct csserver *serv, int tcpudp, u_short port, u_long addr)
{
	int error;

    serv->prompt = s_serv_prompt;

    serv->hsock = cssock_open(tcpudp);
    printf("%s hsock() is OK!\n", serv->prompt);

    if (port == 0) {
        cssock_close(serv->hsock);
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s invalid port.\n", serv->prompt);
    }

    serv->sa_in.sin_family = AF_INET;
    serv->sa_in.sin_port = htons(port);
    serv->sa_in.sin_addr.s_addr = addr;

	cssock_bind(serv->hsock, (struct sockaddr*)&serv->sa_in, sizeof(struct sockaddr_in));
	printf("%s bind() is OK\n", serv->prompt);
}

ssize_t csserver_recv(cssock_t handle, void* inbuf, size_t inbytes)
{
    struct sockaddr cliaddr;
    cssocklen_t addrlen = sizeof(cliaddr);
    ssize_t recvbytes;

    if ((recvbytes = recvfrom(handle, inbuf, inbytes, 0, &cliaddr, &addrlen)) < 0) {
        fprintf(stderr, "server: recvfrom() fail, error code: %d.\n", cssock_get_last_error());
        return -1;
    } else if (recvbytes == 0) {
        fprintf(stdout, "server: peer shutdown, recvfrom() failed.\n");
        return 0;
    }
    csmsg_copyaddr((struct csmsg_header*)inbuf, &cliaddr, addrlen);

#ifdef _DEBUG
    {
        char addrstr[INET6_ADDRSTRLEN + 1];
        printf("server: recefrom() client ip: %s, port: %d.\n",
               cssock_inet_ntop(AF_INET, &((struct sockaddr_in*)&cliaddr)->sin_addr, addrstr, sizeof(addrstr)),
               htons(((struct sockaddr_in*)&cliaddr)->sin_port));
    }
#endif

#ifdef _DEBUG
    {
        static int recvcount = 0;
        printf("server: recvcount %d.\n", recvcount++);
    }
#endif

    return recvbytes - sizeof(struct csmsg_header);
}

void csserver_send(cssock_t handle, void* sendbuf)
{
    ssize_t sendbytes;
	struct csmsg_header* msghdr = NULL;
    uint32_t msgdatalen = ntohl(GET_HEADER_DATA(sendbuf, numbytes));

    msghdr = (struct csmsg_header*)sendbuf;

#ifdef _DEBUG
    {
        char addrstr[INET6_ADDRSTRLEN + 1];
        printf("server: client ip: %s, port: %d.\n",
               cssock_inet_ntop(AF_INET, &((struct sockaddr_in*)&msghdr->addr)->sin_addr, addrstr, sizeof(addrstr)),
               htons(((struct sockaddr_in*)&msghdr->addr)->sin_port));
    }
#endif

    sendbytes = sendto(handle, sendbuf, sizeof(struct csmsg_header) + msgdatalen, 0, &msghdr->addr, ntohl(msghdr->addrlen));
    if (sendbytes < 0) {
        fprintf(stderr, "server: sendto() fail, error code: %d.\n", cssock_get_last_error());
    } else if (sendbytes != (ssize_t)(sizeof(struct csmsg_header) + msgdatalen)) {
        printf("server: sendto() does not send right number of data.\n");
    }
}

