/**
 * @file server.c
 * @brief  The functions prefexed with "s_" are static functions.
 * @author cxl
 * @version 0.1
 * @date 2015-09-28
 * @modified  Wed 2015-11-04 19:13:30 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#else
#include  <setjmp.h>
#include  <signal.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#endif

#include  <stdlib.h>
#include  <stdio.h>
#include  <stdint.h>
#include    "error.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "sock_wrap.h"
#include    "sendrecv_pool.h"
#include    "msgwrap.h"
#include 	"server.h"


#ifdef __cplusplus
extern "C" {
#endif

static char* s_serv_msgheader = "server:";

#ifdef __cplusplus
}
#endif


void csserver_init(struct csserver *serv, int tcpudp, u_short port, u_long addr)
{
	int error;

	serv->msgheader = s_serv_msgheader;

    serv->hsock = cssock_open(tcpudp);
    printf("%s hsock() is OK!\n", serv->msgheader);

    if (port == 0) {
        cssock_close(serv->hsock);
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s invalid port.\n", serv->msgheader);
    }

    serv->sa_in.sin_family = AF_INET;
    serv->sa_in.sin_port = htons(port);
    serv->sa_in.sin_addr.s_addr = addr;

	cssock_bind(serv->hsock, (struct sockaddr*)&serv->sa_in, sizeof(struct sockaddr_in));
	printf("%s bind() is OK\n", serv->msgheader);
}

ssize_t csserver_recv(cssock_t handle, void* inbuf, size_t inbytes)
{
    ssize_t recvbytes;
    if ((recvbytes = recvfrom(handle, inbuf, inbytes, 0, NULL, NULL)) < 0) {
        fprintf(stderr, "server: recvfrom() fail, error code: %d.\n", cssock_get_last_error());
        return -1;
    } else if (recvbytes == 0) {
        fprintf(stdout, "server: peer shutdown, recvfrom() failed.\n");
        return 0;
    }

#ifdef _DEBUG
    {
        static int recvcount = 0;
        printf("server: recvcount %d.\n", recvcount++);
    }
#endif

    return recvbytes - sizeof(struct csmsg_header);
}

void csserver_send(cssock_t handle, const void* outbuf)
{
    ssize_t sendbytes;
	const struct csmsg_header* msghdr = NULL;

	msghdr = (const struct csmsg_header*)outbuf;

#ifdef _DEBUG
    printf("server: client ip: %s, port: %d.\n",
           inet_ntoa(((const struct sockaddr_in*)&msghdr->addr)->sin_addr),
           htons(((const struct sockaddr_in*)&msghdr->addr)->sin_port));
#endif

    sendbytes = sendto(handle, outbuf, sizeof(struct csmsg_header) + msghdr->numbytes, 0, &msghdr->addr, msghdr->addrlen);
    if (sendbytes < 0) {
        fprintf(stderr, "server: sendto() fail, error code: %d.\n", cssock_get_last_error());
    } else if (sendbytes != ((ssize_t)sizeof(struct csmsg_header) + msghdr->numbytes)) {
        printf("server: sendto() does not send right number of data.\n");
    }
}

