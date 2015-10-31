/**
 * @file server_servroutine.c
 * @brief This file must be reimplement when config parser was done.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
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
#include	"server_udp.h"
#include    "server_servroutine.h"
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
 * @param socket
 */
static void s_init_sendpool(SOCKET socket);

/**
 * @brief  s_init_recvpool should parse the config first, and then set data to s_recvpool.
 *
 * @param socket
 */
static void s_init_recvpool(SOCKET socket);

/**
 * @brief  s_process_recv 
 *
 * @param serv_udp
 *
 * @return   
 */
static unsigned int __stdcall s_process_recv(void* unused);

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
    struct unit_header unithdr;
    char buf[MAX_MSG_LEN];

    s_init_sendpool(serv_udp->socket);
    s_init_recvpool(serv_udp->socket);

    printf("%s: I\'m ready to receive a datagram...\n", serv_udp->msgheader);
    while (1) {
        unithdr.addrlen = sizeof(unithdr.addr);
        unithdr.numbytes = server_recv(
                    serv_udp->socket,
                    buf,
                    sizeof(buf),
                    &unithdr.header,
                    (struct sockaddr*)&unithdr.addr,
                    &unithdr.addrlen);
        if (unithdr.numbytes > 0) {

#ifdef _DEBUG
            printf("%s: total bytes received: %d\n", serv_udp->msgheader, unithdr.numbytes);
            printf("%s: the data is \"%s\"\n", serv_udp->msgheader, buf);
#endif
            push2pool(buf, &unithdr, &s_recvpool);
        }
        else if (unithdr.numbytes <= 0) {
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

void s_init_sendpool(SOCKET socket)
{
    init_sendrecv_pool(&s_sendpool, MAX_MSG_LEN + sizeof(struct unit_header), 64, 4, socket, s_process_send);
    s_sendpool.init_pool(&s_sendpool);
}

void s_init_recvpool(SOCKET socket)
{
    init_sendrecv_pool(&s_recvpool, MAX_MSG_LEN + sizeof(struct unit_header), 64, 4, socket, s_process_recv);
    s_recvpool.init_pool(&s_recvpool);
}

unsigned int __stdcall s_process_recv(void* unused)
{
    char msgdata[MAX_MSG_LEN];
    char outmsg[MAX_MSG_LEN];
    struct unit_header unithdr;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(s_recvpool.hsem_filled, INFINITE);

        if (pull_from_pool(msgdata, sizeof(msgdata), &unithdr, &s_recvpool) != 0) {
            continue;
        }

#ifdef WIN32
        printf("recv- thread id: %ld, process message: %s.\n", GetCurrentThreadId(), msgdata);
#endif

        process_msg(msgdata, outmsg, sizeof(outmsg));
        push2pool(outmsg, &unithdr, &s_sendpool);
    }

    return 0;
}

unsigned int __stdcall s_process_send(void* unused)
{
    char outmsg[MAX_MSG_LEN];
    struct unit_header unithdr;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(s_sendpool.hsem_filled, INFINITE);

        if (pull_from_pool(outmsg, sizeof(outmsg), &unithdr, &s_sendpool) != 0) {
            continue;
        }

#ifdef _DEBUG
        getpeername(s_sendpool.socket, (SOCKADDR*)&unithdr.addr, &unithdr.addrlen);
        printf("server: sending IP used: %s\n", inet_ntoa(unithdr.addr.sin_addr));
        printf("server: sending port used: %d\n", htons(unithdr.addr.sin_port));
#endif

        server_send(
                    s_sendpool.socket,
                    outmsg,
                    strlen(outmsg) + 1,
                    &unithdr.header,
                    (SOCKADDR*)&unithdr.addr,
                    unithdr.addrlen);
        Sleep(200);

    }

    return 0;
}
