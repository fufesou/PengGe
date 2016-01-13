/**
 * @file server.c
 * @brief  The functions prefexed with "s_" are static functions.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-28
 * @modified  Wed 2016-01-06 22:47:20 (+0800)
 */

#ifdef WIN32
#include  <ws2tcpip.h>
#include  <winsock2.h>
#include  <windows.h>
#include  <process.h>
#else
#include  <setjmp.h>
#include  <signal.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <unistd.h>
#endif

#ifndef _MSC_VER /* *nix */
#include  <semaphore.h>
#endif

#include  <assert.h>
#include  <stdlib.h>
#include  <stdio.h>
#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/jxiot.h"
#include    "common/utility_wrap.h"
#include    "common/error.h"
#include    "common/bufarray.h"
#include    "common/list.h"
#include    "common/clearlist.h"
#include    "common/processlist.h"
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "common/sock_wrap.h"
#include    "common/msgwrap.h"
#include    "cs/msgpool.h"
#include    "cs/msgpool_dispatch.h"
#include    "cs/server.h"


static char* s_serv_prompt = "server:";

/**
 * @brief s_msgpool_dispatch This variable is used to manage server send and recv pool.
 */
static struct jxmsgpool_dispatch s_msgpool_dispatch;

#ifdef __cplusplus
extern "C" {
#endif

static void s_jxserver_clear(void* serv);
static void s_clear_msgpool_dispatch(void* unused);

#ifdef __cplusplus
}
#endif


void jxserver_init(struct jxserver *serv, int tcpudp, u_short port, u_long addr)
{
    int error;

    serv->prompt = s_serv_prompt;

    serv->hsock = jxsock_open(tcpudp);
    printf("%s hsock() is OK!\n", serv->prompt);

    if (port == 0) {
        jxsock_close(serv->hsock);
        error = 1;
        jxfatal_ext(&error, jxerr_exit, "%s invalid port.\n", serv->prompt);
    }

    serv->sa_in.sin_family = AF_INET;
    serv->sa_in.sin_port = htons(port);
    serv->sa_in.sin_addr.s_addr = addr;

    jxsock_bind(serv->hsock, (struct sockaddr*)&serv->sa_in, sizeof(struct sockaddr_in));
    printf("%s bind() is OK\n", serv->prompt);

    jxclearlist_add(s_jxserver_clear, serv);
}

void s_jxserver_clear(void* serv)
{
    jxsock_close(((struct jxserver*)serv)->hsock);
}

ssize_t jxserver_recv(jxsock_t handle, void* inbuf, size_t inbytes)
{
    struct sockaddr cliaddr;
    jxsocklen_t addrlen = sizeof(cliaddr);
    ssize_t recvbytes;

    if ((recvbytes = recvfrom(handle, inbuf, inbytes, 0, &cliaddr, &addrlen)) < 0) {
        fprintf(stderr, "server: recvfrom() fail, error code: %d.\n", jxsock_get_last_error());
        return -1;
    } else if (recvbytes == 0) {
        fprintf(stdout, "server: peer shutdown, recvfrom() failed.\n");
        return 0;
    }
    jxmsg_copyaddr((struct jxmsg_header*)inbuf, &cliaddr, (uint8_t)addrlen);

#ifdef _DEBUG
    {
        char addrstr[INET6_ADDRSTRLEN + 1];
        printf("server: recefrom() client ip: %s, port: %d.\n",
               jxsock_inet_ntop(AF_INET, &((struct sockaddr_in*)&cliaddr)->sin_addr, addrstr, sizeof(addrstr)),
               ntohs(((struct sockaddr_in*)&cliaddr)->sin_port));
    }
#endif

    return recvbytes - sizeof(struct jxmsg_header);
}

#ifdef _DEBUG
#include    "common/regportlist.h"
#endif

int jxserver_send(jxsock_t handle, void* sendbuf)
{
    ssize_t sendbytes;
    struct jxmsg_header* msghdr = NULL;
    uint32_t msgdatalen = ntohl(GET_HEADER_DATA(sendbuf, numbytes));

    msghdr = (struct jxmsg_header*)sendbuf;

#ifdef _DEBUG
    {
        char addrstr[INET6_ADDRSTRLEN + 1];
        printf("server: client ip: %s, port: %d.\n",
               jxsock_inet_ntop(AF_INET, &((struct sockaddr_in*)&msghdr->addr)->sin_addr, addrstr, sizeof(addrstr)),
               ntohs(((struct sockaddr_in*)&msghdr->addr)->sin_port));
    }
#endif

    sendbytes = sendto(handle, sendbuf, sizeof(struct jxmsg_header) + msgdatalen, 0, &msghdr->addr, (jxsocklen_t)msghdr->addrlen);
    if (sendbytes < 0) {
        fprintf(stderr, "server: sendto() fail, error code: %d.\n", jxsock_get_last_error());
        return 1;
    } else if (sendbytes != (ssize_t)(sizeof(struct jxmsg_header) + msgdatalen)) {
        printf("server: sendto() does not send right number of data.\n");
        return 1;
    }

#ifdef _DEBUG
    {
        unsigned short port;
        if (jxregportlist_query(&((struct sockaddr_in*)&msghdr->addr)->sin_addr, &port)) {
            printf("not registered!\n");
        } else {
        ((struct sockaddr_in*)&msghdr->addr)->sin_port = port;
        sendbytes = sendto(handle, "hello", 6, 0, &msghdr->addr, (jxsocklen_t)msghdr->addrlen);
        if (sendbytes != 6) {
            fprintf(stderr, "server: sendto() fail, error code: %d.\n", jxsock_get_last_error());
        }
        }
    }
#endif

    return 0;
}

void jxserver_udp(struct jxserver* serv)
{
    char* buf = NULL;
    int numbytes = 0;
    struct jxmsgpool* recvpool = &s_msgpool_dispatch.pool_unprocessed;

    printf("%s I\'m ready to receive a datagram...\n", serv->prompt);
    while (1) {

        /** @brief block untile one buffer avaliable. */
        while ((buf = jxpool_pullitem(recvpool, &recvpool->empty_buf)) == NULL)
                    ;

        numbytes = jxserver_recv(serv->hsock, buf, recvpool->len_item);
        if (numbytes >= 0) {

            /** Push to pool will succeed in normal case. There is no need to test the return value. */
            jxpool_pushitem(recvpool, &recvpool->filled_buf, buf);
            jxsem_post(&recvpool->hsem_filled);
        }
        else {
            printf("%s recvfrom() failed with error code: %d\n", serv->prompt, jxsock_get_last_error());
            break;
        }
    }

    printf("%s finish receiving. closing the listening socket...\n", serv->prompt);
}

void s_clear_msgpool_dispatch(void* unused)
{
    (void)unused;
    if (s_msgpool_dispatch.pool_unprocessed.num_thread != 0) {
        jxpool_clear(&s_msgpool_dispatch.pool_unprocessed);
    }
    if (s_msgpool_dispatch.pool_processed.num_thread != 0) {
        jxpool_clear(&s_msgpool_dispatch.pool_processed);
    }
}

JXIOT_API void jxserver_msgpool_dispatch_init(struct jxserver* serv, struct list_head* plist_head)
{
    if (!jxprocesslist_process_valid(plist_head)) {
        jxwarning("invalid process list.");
        return;
    }

    jxmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "server msgpool_dispatch:";
    s_msgpool_dispatch.processlist_head = plist_head;

    jxpool_init(
                &s_msgpool_dispatch.pool_unprocessed,           /* struct jxmsgpool* pool */
                MAX_MSG_LEN + sizeof(struct jxmsg_header),      /* int itemlen            */
                SERVER_POOL_NUM_ITEM,                           /* int itemnum            */
                NUM_THREAD,                                     /* int threadnum          */
                (char*)(&serv->hsock),                       	/* char* userdatsa        */
                sizeof(serv->hsock),							/* size_t size_userdata   */
                jxmsgpool_process,                              /* pthread_proc_t proc    */
                (void*)&s_msgpool_dispatch);                    /* void* pargs            */

    jxclearlist_add(s_clear_msgpool_dispatch, NULL);

    if (!jxprocesslist_process_af_valid(plist_head)) {
        jxwarning("server do not register process_af functions.\n");
        return ;
    }

    jxpool_init(
                &s_msgpool_dispatch.pool_processed,
                MAX_MSG_LEN + sizeof(struct jxmsg_header),
                SERVER_POOL_NUM_ITEM,
                NUM_THREAD,
                (char*)(&serv->hsock),
                sizeof(serv->hsock),
                jxmsgpool_process_af,
                (void*)&s_msgpool_dispatch);
}
