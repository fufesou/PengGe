/**
 * @file server.c
 * @brief  The functions prefexed with "s_" are static functions.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-28
 * @modified  Tue 2015-12-08 23:46:56 (+0800)
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
#include    "common/macros.h"
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
static struct csmsgpool_dispatch s_msgpool_dispatch;

void s_csserver_clear(void* serv);
static void s_init_msgpool_dispatch(struct csserver* serv);
static void s_clear_msgpool_dispatch(void* unused);

static int s_msg_process(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static int s_msg_process_af(char* userdata, char* msg);

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

    s_init_msgpool_dispatch(serv);

    csclearlist_add(s_clear_msgpool_dispatch, NULL);
    csclearlist_add(s_csserver_clear, serv);
}

void s_csserver_clear(void* serv)
{
    cssock_close(((struct csserver*)serv)->hsock);
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

int csserver_send(cssock_t handle, void* sendbuf)
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

    sendbytes = sendto(handle, sendbuf, sizeof(struct csmsg_header) + msgdatalen, 0, &msghdr->addr, msghdr->addrlen);
    if (sendbytes < 0) {
        fprintf(stderr, "server: sendto() fail, error code: %d.\n", cssock_get_last_error());
        return 1;
    } else if (sendbytes != (ssize_t)(sizeof(struct csmsg_header) + msgdatalen)) {
        printf("server: sendto() does not send right number of data.\n");
        return 1;
    }

    return 0;
}

void csserver_udp(struct csserver* serv)
{
    char* buf = NULL;
    int numbytes = 0;
    struct csmsgpool* recvpool = &s_msgpool_dispatch.pool_unprocessed;

    printf("%s: I\'m ready to receive a datagram...\n", serv->prompt);
    while (1) {

        /** @brief block untile one buffer avaliable. */
        while ((buf = cspool_pullitem(recvpool, &recvpool->empty_buf)) == NULL)
                    ;

        numbytes = csserver_recv(serv->hsock, buf, recvpool->len_item);
        if (numbytes > 0) {

            /** Push to pool will succeed in normal case. There is no need to test the return value. */
            cspool_pushitem(recvpool, &recvpool->filled_buf, buf);
            cssem_post(&recvpool->hsem_filled);
        }
        else if (numbytes <= 0) {
            printf("%s: connection closed with error code: %d\n", serv->prompt, cssock_get_last_error());
            break;
        }
        else {
            printf("%s: recvfrom() failed with error code: %d\n", serv->prompt, cssock_get_last_error());
            break;
        }
    }

    printf("%s: finish receiving. closing the listening socket...\n", serv->prompt);
}

void s_init_msgpool_dispatch(struct csserver* serv)
{   
    csmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "server msgpool_dispatch:";
    s_msgpool_dispatch.process_msg = s_msg_process;
    s_msgpool_dispatch.process_af_msg = s_msg_process_af;

    cspool_init(
                &s_msgpool_dispatch.pool_unprocessed,           /**< struct csmsgpool* pool */
                MAX_MSG_LEN + sizeof(struct csmsg_header),      /**< int itemlen            */
                SERVER_POOL_NUM_ITEM,                           /**< int itemnum            */
                NUM_THREAD,                                     /**< int threadnum          */
                (char*)(&serv->hsock),                       	/**< char* userdatsa        */
                sizeof(serv->hsock),							/**< size_t size_userdat    */
                csmsgpool_process,                              /**< pthread_proc_t proc    */
                (void*)&s_msgpool_dispatch);                    /**< void* pargs            */

    cspool_init(
                &s_msgpool_dispatch.pool_processed,
                MAX_MSG_LEN + sizeof(struct csmsg_header),
                SERVER_POOL_NUM_ITEM,
                NUM_THREAD,
                (char*)(&serv->hsock),
                sizeof(serv->hsock),
                csmsgpool_process_af,
                (void*)&s_msgpool_dispatch);

    csclearlist_add(s_clear_msgpool_dispatch, NULL);
}

void s_clear_msgpool_dispatch(void* unused)
{
    (void)unused;
    cspool_clear(&s_msgpool_dispatch.pool_unprocessed);
    cspool_clear(&s_msgpool_dispatch.pool_processed);
}

/**
 * @brief  s_msg_dispatch message will be dispatched to account operations.
 *
 * @param inmsg The format of inmsg is \n
 * -----------------------------------------------------------------------------------------\n
 * | struct csmsg_header | process id(uint32_t) | data(char*) | ... |                       \n
 * -----------------------------------------------------------------------------------------\n
 *
 * @outmsg
 * @outmsglen
 *
 * @return   
 */
int s_msg_process(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    int ret = 0;
    uint32_t id_process = -1;
    static uint32_t s_fixedlen = sizeof(struct csmsg_header) + sizeof(uint32_t);
    if (*outmsglen < s_fixedlen)
    {
        return 1;
    }

    cs_memcpy(outmsg, *outmsglen, inmsg, s_fixedlen);
    *outmsglen -= s_fixedlen;

    id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));
    ret = am_method_get(id_process)->reply(
                inmsg + s_fixedlen,
                &((struct csmsg_header*)inmsg)->addr,
                ((struct csmsg_header*)inmsg)->addrlen,
                outmsg + s_fixedlen,
                outmsglen);

    /** The length of variables.
     *
     * - s_fixedlen is only used here as a help variable, and stands for nothing.
     * - *outmsglen is the same as length of total message(without struct csmsg_header).
     * - 'numbytes' in 'struct csmsg_header' is the same as *outmsglen.
     */
    *outmsglen += sizeof(uint32_t);
    ((struct csmsg_header*)outmsg)->numbytes = htonl(*outmsglen);
    return ret;
}

int s_msg_process_af(char* userdata, char* msg)
{
    return csserver_send(*(cssock_t*)userdata, msg);
}
