/**
 * @file client.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-30
 * @modified  Fri 2016-01-01 18:39:42 (+0800)
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

struct csmsgpool;

#include  <assert.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/jxiot.h"
#include    "common/list.h"
#include    "common/error.h"
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
#include    "am/account.h"


struct recv_params_t {
    struct csclient* pcli;
    const struct sockaddr* pservaddr;
    cssocklen_t addrlen;
};

#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_exit;
extern int g_cli_sendbuflen;
extern int g_cli_recvbuflen;

static char* s_cli_prompt = "client:";

/** s_msgpool_dispatch This variable is used to manage client send and recv pool. */
static struct csmsgpool_dispatch s_msgpool_dispatch;

static int s_isRecvEnd = 0;
static csthread_t s_recv_thread;

static void s_finish_thread_recv(void);
static void s_thread_recv_clear(void*);
static void s_msgpool_append(char* data, ssize_t numbytes);

#ifdef WIN32
static unsigned int __stdcall s_thread_recv(void*);
#else
static void* CS_CALLBACK s_thread_recv(void*);
#endif

static void s_msgpool_append(char* data, ssize_t numbytes);
void s_clear_msgpool_dispatch(void* unused);

/**
 * @brief  s_react_dispatch The income message from server
 * will be handle by the coressponding 'react' method in this function.
 *
 * The actual process function will not process data until one byte after 'process id'. Thus
 * the message data passed into the process function is 'inmsg + sizeof(struct csmsg_header) + sizeof(uint32_t)'.
 *
 * @param inmsg The format of inmsg is: \n
 * ---------------------------------------------------------------------------------------------- \n
 * | struct csmsg_header | process id(uint32_t) | process data(char*) | ... |                     \n
 * ---------------------------------------------------------------------------------------------- \n
 * or \n
 * ------------------------------------------------------------------------------------------------------------------------ \n
 * | struct csmsg_header | process id(uint32_t) | user id(uint32_t) | process data(char*) | ...                             \n
 * ------------------------------------------------------------------------------------------------------------------------ \n
 * @param outmsg
 * @param outmsglen
 */
static int s_react_dispatch(char* inmsg, char* outmsg, __csinout uint32_t* outmsglen);

#ifdef __cplusplus
}
#endif


void csclient_init(struct csclient* cli, int tcpudp)
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

    cli->hsock_sendrecv = cssock_open(tcpudp);
    if (cssock_block(cli->hsock_sendrecv, nonblocking) != 0) {
        error = JX_NORMAL_ERR;
        csfatal_ext(&error, cserr_exit, "%s: set socket to non-blocking mode failed,"
                    " error code: %d\n", cli->prompt, cssock_get_last_error());
    }

#ifdef WIN32
    cli->hsock_recv = INVALID_SOCKET;
#else
    cli->hsock_recv = -1;
#endif

    printf("%s: socket() is OK!\n", cli->prompt);
}

void csclient_clear(void* cli)
{
    struct csclient* pcli = (struct csclient*)cli;
    if (IS_SOCK_HANDLE(pcli->hsock_sendrecv))
        cssock_close(pcli->hsock_sendrecv);
    if (IS_SOCK_HANDLE(pcli->hsock_recv))
        cssock_close(pcli->hsock_recv);

    /*
     * @par free memory
     * @code{.c}
     * struct csclient* lcli = cli;
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

int s_react_dispatch(char* inmsg, char* outmsg, __csinout uint32_t* outmsglen)
{
    uint32_t id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));
    return am_method_get(id_process)->react(inmsg + sizeof(struct csmsg_header) + sizeof(uint32_t), outmsg, outmsglen);
}

int csclient_print(cssock_t hsock, const char* prompt)
{
    int testres;
    cserr_t error;
    if ((testres = cssock_print(hsock, prompt)) == JX_INVALIE_ARGS) {
        error = JX_INVALIE_ARGS;
        csfatal_ext(&error, cserr_exit, "%s socket has not been created.\n", prompt);
    } else if (testres == JX_WARNING) {
        printf("%s socket has not been connected.\n", prompt);
    }

    return testres;
}

void csclient_connect(cssock_t hsock, const char* prompt, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    cssock_connect(hsock, servaddr, addrlen);
    if (csclient_print(hsock, prompt) != 0) {
        csclearlist_clear();
        exit(JX_FATAL);
    }
}

void csclient_udp(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    csclient_connect(cli->hsock_sendrecv, cli->prompt, servaddr, addrlen);
    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';

        if (strncmp(cli->sendbuf, g_exit, strlen(g_exit) + 1) == 0) {
            printf("%s exit.\n", cli->prompt);
            break;
        }

        csclient_udp_once(cli, servaddr, addrlen);
    }
}

void csclient_udp_once(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    ssize_t numbytes = csclient_sendrecv(cli, servaddr, addrlen);
    if (numbytes > 0) {
        s_msgpool_append(cli->recvbuf, numbytes);
    }
}

void s_msgpool_append(char* data, ssize_t numbytes)
{
    char* buf = NULL;
    struct csmsgpool* recvpool = &s_msgpool_dispatch.pool_unprocessed;

    if (numbytes > 0) {
        /** block untile one buffer avaliable. */
        while ((buf = cspool_pullitem(recvpool, &recvpool->empty_buf)) == NULL)
                ;

        cs_memcpy(buf, recvpool->len_item, data, numbytes + sizeof(struct csmsg_header));

        /** Push to pool will succeed in normal case. There is no need to test the return value.
         */
        cspool_pushitem(recvpool, &recvpool->filled_buf, buf);
        cssem_post(&recvpool->hsem_filled);
    }
}

void csclient_msgpool_dispatch_init(pfunc_msgprocess_t func_msgprocess, pfunc_msgprocess_af_t func_msgprocess_af)
{   
    const int threadnum = 1; 

    csmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "client msgpool_dispatch:";
    s_msgpool_dispatch.process_msg = (func_msgprocess == NULL) ? s_react_dispatch : func_msgprocess;
    s_msgpool_dispatch.process_af_msg = func_msgprocess_af;

    cspool_init(
                &s_msgpool_dispatch.pool_unprocessed,       /* struct csmsgpool* pool  */
                MAX_MSG_LEN + sizeof(struct csmsg_header),  /* int itemlen             */
                SERVER_POOL_NUM_ITEM,                       /* int itemnum             */
                threadnum,                                  /* int threadnum           */
                0,					                       	/* char* userdata          */
                0,											/* size_t size_userdata    */
                csmsgpool_process,                          /* pthread_proc_t proc     */
                (void*)&s_msgpool_dispatch);                /* void* pargs             */

    if (func_msgprocess_af != NULL) {
        cspool_init(
                    &s_msgpool_dispatch.pool_processed,
                    MAX_MSG_LEN + sizeof(struct csmsg_header),
                    SERVER_POOL_NUM_ITEM,
                    threadnum,
                    0,
                    0,
                    csmsgpool_process_af,
                    (void*)&s_msgpool_dispatch);
    }

    csclearlist_add(s_clear_msgpool_dispatch, NULL);
}

void s_clear_msgpool_dispatch(void* unused)
{
    (void)unused;
    cspool_clear(&s_msgpool_dispatch.pool_unprocessed);
}

int csclient_thread_recv(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    struct recv_params_t recv_params = { cli, servaddr, addrlen };
    return csthread_create(s_thread_recv, &recv_params, &s_recv_thread);
}

void s_finish_thread_recv(void)
{
    s_isRecvEnd = 1;
}

#ifdef WIN32
unsigned int __stdcall s_thread_recv(void* recv_params)
#else
void* CS_CALLBACK s_thread_recv(void* recv_params)
#endif
{
    char* pbuf = NULL;
    int blocking = 0;
    ssize_t recvlen = 0;
    struct recv_params_t* precv_params = recv_params;

    precv_params->pcli->hsock_recv = cssock_open(SOCK_DGRAM);
    if (cssock_block(precv_params->pcli->hsock_recv, blocking) != 0) {
        fprintf(stderr, "client: set socket to blocking mode failed, error code: %d\n", cssock_get_last_error());
#ifdef WIN32
        return JX_NORMAL_ERR;
#else
        return (void*)JX_NORMAL_ERR;
#endif
    }

    csclearlist_add(s_thread_recv_clear, NULL);
    pbuf = (char*)malloc(precv_params->pcli->size_recvbuf);

    csclient_connect(precv_params->pcli->hsock_recv, precv_params->pcli->prompt, precv_params->pservaddr, precv_params->addrlen);

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
    csthread_wait_terminate(s_recv_thread);
}
