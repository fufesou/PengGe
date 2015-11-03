/**
 * @file server_udp.c
 * @brief This file must be reimplement when config parser was done.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 * @modified  Tue 2015-11-03 19:51:37 (+0800)
 */

#include  <assert.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <winsock2.h>
#include  <windows.h>
#include  <process.h>
#include  <semaphore.h>
#include	"macros.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "server_sendrecv.h"
#include    "msgdispatch.h"
#include    "sendrecv_pool.h"
#include	"msgwrap.h"


#ifdef __cplusplus
extern "C" {
#endif

static struct sendrecv_pool s_sendpool, s_recvpool;


/**
 * @brief  s_init_sendpool should parse the config first, and then set data to s_sendpool.
 *
 * @param handle
 */
static void s_init_sendpool(cssock_t handle);

/**
 * @brief  s_init_recvpool should parse the config first, and then set data to s_recvpool.
 *
 * @param handle
 */
static void s_init_recvpool(cssock_t handle);

/**
 * @brief  s_process_recv 
 *
 * @return   
 */
#ifdef WIN32
static unsigned int __stdcall s_process_recv(void* unused);
#else
static void* s_process_recv(void* unused);
#endif


/**
 * @brief  s_process_send 
 *
 * @param serv_udp
 *
 * @return   
 */
static unsigned int __stdcall s_process_send(void* unused);

#ifdef __cplusplus
}
#endif

void process_communication(struct server_udp* serv_udp)
{
    struct csmsg_header msghdr;
    char buf[MAX_MSG_LEN];

    s_init_sendpool(serv_udp->hsock);
    s_init_recvpool(serv_udp->hsock);

    printf("%s: I\'m ready to receive a datagram...\n", serv_udp->msgheader);
    while (1) {
        msghdr.addrlen = sizeof(msghdr.addr);
        msghdr.numbytes = server_recv(
                    serv_udp->socket,
                    buf,
                    sizeof(buf),
                    &msghdr.header,
                    (struct sockaddr*)&msghdr.addr,
                    &msghdr.addrlen);
        if (msghdr.numbytes > 0) {

#ifdef _DEBUG
            printf("%s: total bytes received: %d\n", serv_udp->msgheader, msghdr.numbytes);
            printf("%s: the data is \"%s\"\n", serv_udp->msgheader, buf);
#endif
            push2pool(buf, &msghdr, &s_recvpool);
        }
        else if (msghdr.numbytes <= 0) {
            printf("%s: connection closed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
        else {
            printf("%s: recvfrom() failed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
    }

    printf("%s: finish receiving. closing the listening socket...\n", serv_udp->msgheader);

    s_sendpool.clear_pool(&s_sendpool);
    s_recvpool.clear_pool(&s_recvpool);
}

void s_init_sendpool(cssock_t handle)
{
    cspool_init(&s_sendpool, MAX_MSG_LEN + sizeof(struct csmsg_header), 64, 4, socket, s_process_send);
}

void s_init_recvpool(cssock_t socket)
{
    init_sendrecv_pool(&s_recvpool, MAX_MSG_LEN + sizeof(struct csmsg_header), 64, 4, socket, s_process_recv);
    s_recvpool.init_pool(&s_recvpool);
}

unsigned int __stdcall s_process_recv(void* unused)
{
    char msgdata[MAX_MSG_LEN];
    char outmsg[MAX_MSG_LEN];
    struct csmsg_header msghdr;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(s_recvpool.hsem_filled, INFINITE);

        if (pull_from_pool(msgdata, sizeof(msgdata), &msghdr, &s_recvpool) != 0) {
            continue;
        }

#ifdef WIN32
        printf("recv- thread id: %ld, process message: %s.\n", GetCurrentThreadId(), msgdata);
#endif

        process_msg(msgdata, outmsg, sizeof(outmsg));
        push2pool(outmsg, &msghdr, &s_sendpool);
    }

    return 0;
}

unsigned int __stdcall s_process_send(void* unused)
{
    char outmsg[MAX_MSG_LEN];
    struct csmsg_header msghdr;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(s_sendpool.hsem_filled, INFINITE);

        if (pull_from_pool(outmsg, sizeof(outmsg), &msghdr, &s_sendpool) != 0) {
            continue;
        }

#ifdef _DEBUG
        getpeername(s_sendpool.socket, (SOCKADDR*)&msghdr.addr, &msghdr.addrlen);
        printf("server: sending IP used: %s\n", inet_ntoa(msghdr.addr.sin_addr));
        printf("server: sending port used: %d\n", htons(msghdr.addr.sin_port));
#endif

        server_send(
                    s_sendpool.socket,
                    outmsg,
                    strlen(outmsg) + 1,
                    &msghdr.header,
                    (SOCKADDR*)&msghdr.addr,
                    msghdr.addrlen);
        Sleep(200);

    }

    return 0;
}
