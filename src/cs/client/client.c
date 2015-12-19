/**
 * @file client.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-30
 * @modified  Wed 2015-12-09 19:33:58 (+0800)
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
#include    "common/macros.h"
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

#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_exit;
extern int g_cli_sendbuflen;
extern int g_cli_recvbuflen;

static char* s_cli_prompt = "client:";

/** s_msgpool_dispatch This variable is used to manage client send and recv pool. */
static struct csmsgpool_dispatch s_msgpool_dispatch;

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
 * ------------------------------------------------------------------------------------------------------------------------------------ \n
 * | struct csmsg_header | process id(uint32_t) | user id(uint32_t) | process data(char*) | ... |                                       \n
 * ------------------------------------------------------------------------------------------------------------------------------------ \n
 * @param outmsg
 * @param outmsglen
 */
static int s_react_dispatch(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);

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

    cli->hsock = cssock_open(tcpudp);
    if (cssock_block(cli->hsock, nonblocking) != 0) {
        error = 1;
        csfatal_ext(&error, cserr_exit, "%s: set socket to non-blocking mode failed, error code: %d\n", cli->prompt, cssock_get_last_error());
    }

    csclearlist_add(csclient_clear, cli);
    printf("%s: socket() is OK!\n", cli->prompt);
}

void csclient_clear(void* cli)
{
    (void)cli;

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

int s_react_dispatch(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    uint32_t id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));
    return am_method_get(id_process)->react(inmsg + sizeof(struct csmsg_header) + sizeof(uint32_t), outmsg, outmsglen);
}

int csclient_print(const struct csclient *cli)
{
    int testres;
    cserr_t error;
    if ((testres = cssock_print(cli->hsock, cli->prompt)) == -1) {
        error = 1;
        csfatal_ext(&error, cserr_exit, "%s socket has not been created.\n", cli->prompt);
    } else if (testres == 1) {
        printf("%s socket has not been connected.\n", cli->prompt);
    }

    return testres;
}

void csclient_connect(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    cssock_connect(cli->hsock, servaddr, addrlen);
    if (csclient_print(cli) != 0) {
        csclearlist_clear();
        exit(1);
    }
}

void csclient_udp(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    csclient_connect(cli, servaddr, addrlen);
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
                0,					                       	/* char* userd<>tsa        */
                0,											/* size_t size_userdata    */
                csmsgpool_process,                          /* pthread_proc_t proc     */
                (void*)&s_msgpool_dispatch);                /* void* pargs             */

    if (func_msgprocess_af != NULL) {
        cspool_init(
                    &s_msgpool_dispatch.pool_processed,
                    MAX_MSG_LEN + sizeof(struct csmsg_header),
                    SERVER_POOL_NUM_ITEM,
                    NUM_THREAD,
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

