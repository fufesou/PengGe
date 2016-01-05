/**
 * @file server.c
 * @brief  The functions prefexed with "s_" are static functions.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-28
 * @modified  Wed 2015-12-09 19:34:13 (+0800)
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
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "common/sock_wrap.h"
#include    "common/msgwrap.h"
#include    "cs/msgpool.h"
#include    "cs/msgpool_dispatch.h"
#include    "cs/server.h"
#include    "am/account.h"


#ifdef __cplusplus
extern "C" {
#endif

static char* s_serv_prompt = "server:";

/**
 * @brief s_msgpool_dispatch This variable is used to manage server send and recv pool.
 */
static struct jxmsgpool_dispatch s_msgpool_dispatch;

void s_jxserver_clear(void* serv);
static void s_init_msgpool_dispatch(struct jxserver* serv);
static void s_clear_msgpool_dispatch(void* unused);

static int s_msg_process(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen);
static int s_msg_process_af(char* userdata, char* msg);

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

    s_init_msgpool_dispatch(serv);

    jxclearlist_add(s_clear_msgpool_dispatch, NULL);
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
               htons(((struct sockaddr_in*)&cliaddr)->sin_port));
    }
#endif

#ifdef _DEBUG
    {
        static int recvcount = 0;
        printf("server: recvcount %d.\n", recvcount++);
    }
#endif

    return recvbytes - sizeof(struct jxmsg_header);
}

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
               htons(((struct sockaddr_in*)&msghdr->addr)->sin_port));
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

    return 0;
}

void jxserver_udp(struct jxserver* serv)
{
    char* buf = NULL;
    int numbytes = 0;
    struct jxmsgpool* recvpool = &s_msgpool_dispatch.pool_unprocessed;

    printf("%s: I\'m ready to receive a datagram...\n", serv->prompt);
    while (1) {

        /** @brief block untile one buffer avaliable. */
        while ((buf = jxpool_pullitem(recvpool, &recvpool->empty_buf)) == NULL)
                    ;

        numbytes = jxserver_recv(serv->hsock, buf, recvpool->len_item);
        if (numbytes > 0) {

            /** Push to pool will succeed in normal case. There is no need to test the return value. */
            jxpool_pushitem(recvpool, &recvpool->filled_buf, buf);
            jxsem_post(&recvpool->hsem_filled);
        }
        else if (numbytes <= 0) {
            printf("%s: connection closed with error code: %d\n", serv->prompt, jxsock_get_last_error());
            break;
        }
        else {
            printf("%s: recvfrom() failed with error code: %d\n", serv->prompt, jxsock_get_last_error());
            break;
        }
    }

    printf("%s: finish receiving. closing the listening socket...\n", serv->prompt);
}

void s_init_msgpool_dispatch(struct jxserver* serv)
{   
    jxmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "server msgpool_dispatch:";
    s_msgpool_dispatch.process_msg = s_msg_process;
    s_msgpool_dispatch.process_af_msg = s_msg_process_af;

    jxpool_init(
                &s_msgpool_dispatch.pool_unprocessed,           /* struct jxmsgpool* pool */
                MAX_MSG_LEN + sizeof(struct jxmsg_header),      /* int itemlen            */
                SERVER_POOL_NUM_ITEM,                           /* int itemnum            */
                NUM_THREAD,                                     /* int threadnum          */
                (char*)(&serv->hsock),                       	/* char* userdatsa        */
                sizeof(serv->hsock),							/* size_t size_userdat    */
                jxmsgpool_process,                              /* pthread_proc_t proc    */
                (void*)&s_msgpool_dispatch);                    /* void* pargs            */

    jxpool_init(
                &s_msgpool_dispatch.pool_processed,
                MAX_MSG_LEN + sizeof(struct jxmsg_header),
                SERVER_POOL_NUM_ITEM,
                NUM_THREAD,
                (char*)(&serv->hsock),
                sizeof(serv->hsock),
                jxmsgpool_process_af,
                (void*)&s_msgpool_dispatch);

    jxclearlist_add(s_clear_msgpool_dispatch, NULL);
}

void s_clear_msgpool_dispatch(void* unused)
{
    (void)unused;
    jxpool_clear(&s_msgpool_dispatch.pool_unprocessed);
    jxpool_clear(&s_msgpool_dispatch.pool_processed);
}

/**
 * @brief  s_msg_dispatch message will be dispatched to account operations.
 *
 * @param inmsg The format of inmsg is \n
 * -----------------------------------------------------------------------------------------\n
 * | struct jxmsg_header | process id(uint32_t) | data(char*) | ... |                       \n
 * -----------------------------------------------------------------------------------------\n
 *
 * @outmsg
 * @outmsglen
 *
 * @return   
 */
int s_msg_process(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    int ret = 0;
    uint32_t id_process = -1;
    static uint32_t s_fixedlen = sizeof(struct jxmsg_header) + sizeof(uint32_t);
    if (*outmsglen < s_fixedlen)
    {
        return 1;
    }

    jxmemcpy(outmsg, *outmsglen, inmsg, s_fixedlen);
    *outmsglen -= s_fixedlen;

    id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct jxmsg_header)));
    ret = am_method_get(id_process)->reply(
                inmsg + s_fixedlen,
                &((struct jxmsg_header*)inmsg)->addr,
                (uint32_t)((struct jxmsg_header*)inmsg)->addrlen,
                outmsg + s_fixedlen,
                outmsglen);

    /** The length of variables.
     *
     * - s_fixedlen is only used here as a help variable, and stands for nothing.
     * - *outmsglen is the same as length of total message(without struct jxmsg_header).
     * - 'numbytes' in 'struct jxmsg_header' is the same as *outmsglen.
     */
    *outmsglen += sizeof(uint32_t);
    ((struct jxmsg_header*)outmsg)->numbytes = htonl(*outmsglen);
    return ret;
}

int s_msg_process_af(char* userdata, char* msg)
{
    return jxserver_send(*(jxsock_t*)userdata, msg);
}
