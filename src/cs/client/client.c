/**
 * @file client.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-30
 * @modified  Tue 2016-01-05 23:29:24 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#include  <process.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <pthread.h>
#include  <unistd.h>
#endif

#ifndef _MSC_VER  /* *nix */
#include  <semaphore.h>
#endif

struct jxmsgpool;

#include  <assert.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/jxiot.h"
#include    "common/list.h"
#include    "common/error.h"
#include    "common/processlist.h"
#include    "common/bufarray.h"
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "common/utility_wrap.h"
#include    "common/clearlist.h"
#include    "common/sock_wrap.h"
#include    "common/msgwrap.h"
#include    "cs/msgpool.h"
#include    "cs/msgpool_dispatch.h"
#include    "cs/client.h"

struct recv_params_t {
    struct jxclient* pcli;
    const struct sockaddr* pservaddr;
    jxsocklen_t addrlen;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_exit;
extern int g_cli_sendbuflen;
extern int g_cli_recvbuflen;

static char* s_cli_prompt = "client:";

/** s_msgpool_dispatch This variable is used to manage client send and recv pool. */
static struct jxmsgpool_dispatch s_msgpool_dispatch;

static int s_isRecvEnd = 0;
static jxthread_t s_recv_thread;

static void s_finish_thread_recv(void);
static void s_thread_recv_clear(void*);
static void s_msgpool_append(char* data, ssize_t numbytes);

#ifdef WIN32
static unsigned int __stdcall s_thread_recv(void*);
#else
static void* JXIOT_CALLBACK s_thread_recv(void*);
#endif

static void s_msgpool_append(char* data, ssize_t numbytes);
void s_clear_msgpool_dispatch(void* unused);

#ifdef __cplusplus
}
#endif


void jxclient_init(struct jxclient* cli, int tcpudp)
{
    int error;
    int nonblocking = 1;

    cli->prompt = s_cli_prompt;

    /*
     * @par code to be added
     * @code{.c}
     * cli->size_sendbuf = g_cli_sendbuflen;
     * cli->size_recvbuf = g_cli_recvbuflen;
     * @endcode
     *
     */
    cli->size_senbuf = sizeof(cli->sendbuf);
    cli->size_recvbuf = sizeof(cli->recvbuf);

    cli->hsock_sendrecv = jxsock_open(tcpudp);
    if (jxsock_block(cli->hsock_sendrecv, nonblocking) != 0) {
        error = JX_NORMAL_ERR;
        jxfatal_ext(&error, jxerr_exit, "%s: set socket to non-blocking mode failed,"
                    " error code: %d\n", cli->prompt, jxsock_get_last_error());
    }

#ifdef WIN32
    cli->hsock_recv = INVALID_SOCKET;
#else
    cli->hsock_recv = -1;
#endif

    printf("%s: socket() is OK!\n", cli->prompt);
}

void jxclient_clear(void* cli)
{
    struct jxclient* pcli = (struct jxclient*)cli;
    if (IS_SOCK_HANDLE(pcli->hsock_sendrecv))
        jxsock_close(pcli->hsock_sendrecv);
    if (IS_SOCK_HANDLE(pcli->hsock_recv))
        jxsock_close(pcli->hsock_recv);

    /*
     * @par free memory
     * @code{.c}
     * struct jxclient* lcli = cli;
     * if (lcli->sendbuf) {
     *     free(lcli->sendbuf);
     *     lcli->sendbuf = 0;
     * }
     * if (lcli->recvbuf) {
     *     free(lcli->recvbuf);
     *     lcli->recvbuf = 0;
     * }
     * @endcode
     *
     */
}

int jxclient_print(jxsock_t hsock, const char* prompt)
{
    int testres;
    jxerr_t error;
    if ((testres = jxsock_print(hsock, prompt)) == JX_INVALIE_ARGS) {
        error = JX_INVALIE_ARGS;
        jxfatal_ext(&error, jxerr_exit, "%s socket has not been created.\n", prompt);
    } else if (testres == JX_WARNING) {
        printf("%s socket has not been connected.\n", prompt);
    }

    return testres;
}

void jxclient_connect(jxsock_t hsock, const char* prompt, const struct sockaddr* servaddr, jxsocklen_t addrlen)
{
    jxsock_connect(hsock, servaddr, addrlen);
    if (jxclient_print(hsock, prompt) != 0) {
        jxclearlist_clear();
        exit(JX_FATAL);
    }
}

void jxclient_udp(struct jxclient* cli, FILE* fp, const struct sockaddr* servaddr, jxsocklen_t addrlen)
{
    jxclient_connect(cli->hsock_sendrecv, cli->prompt, servaddr, addrlen);
    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';

        if (strncmp(cli->sendbuf, g_exit, strlen(g_exit) + 1) == 0) {
            printf("%s exit.\n", cli->prompt);
            break;
        }

        jxclient_udp_once(cli, servaddr, addrlen);
    }
}

void jxclient_udp_once(struct jxclient* cli, const struct sockaddr* servaddr, jxsocklen_t addrlen)
{
    ssize_t numbytes = jxclient_sendrecv(cli, servaddr, addrlen);
    if (numbytes > 0) {
        s_msgpool_append(cli->recvbuf, numbytes);
    }
}

void s_msgpool_append(char* data, ssize_t numbytes)
{
    char* buf = NULL;
    struct jxmsgpool* recvpool = NULL;

    if (s_msgpool_dispatch.prompt == 0) {
        return;
    }

    recvpool = &s_msgpool_dispatch.pool_unprocessed;

    if (numbytes > 0) {
        /** block untile one buffer avaliable. */
        while ((buf = jxpool_pullitem(recvpool, &recvpool->empty_buf)) == NULL)
                ;

        jxmemcpy(buf, recvpool->len_item, data, numbytes + sizeof(struct jxmsg_header));

        /** Push to pool will succeed in normal case. There is no need to test the return value.
         */
        jxpool_pushitem(recvpool, &recvpool->filled_buf, buf);
        jxsem_post(&recvpool->hsem_filled);
    }
}

void jxclient_msgpool_dispatch_init(struct list_head processlist_head)
{   
    const int threadnum = 1; 

    if (!jxprocesslist_process_valid(&processlist_head)) {
        jxwarning("invalid process list.");
        return;
    }

    jxmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "client msgpool_dispatch:";
    s_msgpool_dispatch.processlist_head = processlist_head;

    jxpool_init(
                &s_msgpool_dispatch.pool_unprocessed,       /* struct jxmsgpool* pool  */
                MAX_MSG_LEN + sizeof(struct jxmsg_header),  /* int itemlen             */
                SERVER_POOL_NUM_ITEM,                       /* int itemnum             */
                threadnum,                                  /* int threadnum           */
                0,					                       	/* char* userdata          */
                0,											/* size_t size_userdata    */
                jxmsgpool_process,                          /* pthread_proc_t proc     */
                (void*)&s_msgpool_dispatch);                /* void* pargs             */

    jxclearlist_add(s_clear_msgpool_dispatch, NULL);

    if (!jxprocesslist_process_af_valid(&processlist_head)) {
        return ;
    }

    jxpool_init(
            &s_msgpool_dispatch.pool_processed,
            MAX_MSG_LEN + sizeof(struct jxmsg_header),
            SERVER_POOL_NUM_ITEM,
            threadnum,
            0,
            0,
            jxmsgpool_process_af,
            (void*)&s_msgpool_dispatch);
}

void s_clear_msgpool_dispatch(void* unused)
{
    (void)unused;
    if (s_msgpool_dispatch.pool_processed.num_thread != 0) {
        jxpool_clear(&s_msgpool_dispatch.pool_processed);
    }
    if (s_msgpool_dispatch.pool_unprocessed.num_thread != 0) {
        jxpool_clear(&s_msgpool_dispatch.pool_unprocessed);
    }
}

int jxclient_thread_recv(struct jxclient* cli, const struct sockaddr* servaddr, jxsocklen_t addrlen)
{
    struct recv_params_t recv_params = { cli, servaddr, addrlen };
    return jxthread_create(s_thread_recv, &recv_params, &s_recv_thread);
}

void s_finish_thread_recv(void)
{
    s_isRecvEnd = 1;
}

#ifdef WIN32
unsigned int __stdcall s_thread_recv(void* recv_params)
#else
void* JXIOT_CALLBACK s_thread_recv(void* recv_params)
#endif
{
    char* pbuf = NULL;
    int blocking = 0;
    ssize_t recvlen = 0;
    struct recv_params_t* precv_params = recv_params;

    precv_params->pcli->hsock_recv = jxsock_open(SOCK_DGRAM);
    if (jxsock_block(precv_params->pcli->hsock_recv, blocking) != 0) {
        fprintf(stderr, "client: set socket to blocking mode failed, error code: %d\n", jxsock_get_last_error());
#ifdef WIN32
        return JX_NORMAL_ERR;
#else
        return (void*)JX_NORMAL_ERR;
#endif
    }

    jxclearlist_add(s_thread_recv_clear, NULL);
    pbuf = (char*)malloc(precv_params->pcli->size_recvbuf);

    jxclient_connect(precv_params->pcli->hsock_recv, precv_params->pcli->prompt, precv_params->pservaddr, precv_params->addrlen);

    while (!s_isRecvEnd) {
        recvlen = recvfrom(precv_params->pcli->hsock_recv, pbuf, precv_params->pcli->size_recvbuf, 0, NULL, NULL);
        s_msgpool_append(pbuf, recvlen);
    }

    free(pbuf);

    return 0;
}

void s_thread_recv_clear(void* unused)
{
    (void)unused;
    s_finish_thread_recv();
    jxthread_wait_terminate(s_recv_thread);
}
