/**
 * @file client.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-30
 * @modified  Thu 2015-11-19 23:37:50 (+0800)
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

struct csmsgpool;

#include  <assert.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <semaphore.h>
#include    "config_macros.h"
#include    "macros.h"
#include    "list.h"
#include    "error.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "utility_wrap.h"
#include    "clearlist.h"
#include    "msgpool.h"
#include    "sock_wrap.h"
#include    "msgwrap.h"
#include    "msgpool_dispatch.h"
#include    "client.h"
#include    "account.h"

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
static void s_init_msgpool_dispatch(struct csclient* cli);
static void s_clear_msgpool_dispatch(void);

#ifdef __cplusplus
}
#endif


void csclient_init(struct csclient* cli, int tcpudp)
{
	int error;
	int nonblocking = 1;

    cli->prompt = s_cli_prompt;
	cli->pfunc_process_react = csclient_react_dispatch;

    /*
     * @par code to be added
     * @code{.c}
     * cli->len_sendbuf = g_cli_sendbuflen;
     * cli->len_recvbuf = g_cli_recvbuflen;
     * @endcode
     *
     */

    cli->hsock = cssock_open(tcpudp);
    if (cssock_block(cli->hsock, nonblocking) != 0) {
		error = 1;
        csfatal_ext(&error, cserr_exit, "%s: set socket to non-blocking mode failed, error code: %d\n", cli->prompt, cssock_get_last_error());
	}

	s_init_msgpool_dispatch(cli);
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

	s_clear_msgpool_dispatch();
}

void csclient_react_dispatch(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    uint32_t id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));
	am_method_get(id_process)->react(inmsg + sizeof(struct csmsg_header), outmsg, outmsglen);
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
		/*
		 * @brief block untile one buffer avaliable.
		 */
		while ((buf = cspool_pullitem(recvpool, &recvpool->empty_buf)) == NULL)
				;

		cs_memcpy(buf, recvpool->len_item, data, numbytes + sizeof(struct csmsg_header));

		/*
		 * @brief Push to pool will succeed in normal case. There is no need to test the return value.
		 */
		cspool_pushitem(recvpool, &recvpool->filled_buf, buf);
		cssem_post(&recvpool->hsem_filled);
	}
}

void s_init_msgpool_dispatch(struct csclient* cli)
{	
	const int threadnum = 1; 

	csmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "client msgpool_dispatch:";
	s_msgpool_dispatch.process_msg = cli->pfunc_process_react;
	s_msgpool_dispatch.process_af_msg = 0;

    cspool_init(
                &s_msgpool_dispatch.pool_unprocessed,		/** struct csmsgpool* pool 	*/
                MAX_MSG_LEN + sizeof(struct csmsg_header),	/** int itemlen 			*/
                SERVER_POOL_NUM_ITEM,						/** int itemnum 			*/
                threadnum,									/** int threadnum			*/
                cli->hsock,									/** cssock_t socket 		*/
                csmsgpool_process,							/** csthread_proc_t proc 	*/
                (void*)&s_msgpool_dispatch);				/** void* pargs 			*/
}

void s_clear_msgpool_dispatch(void)
{
    cspool_clear(&s_msgpool_dispatch.pool_unprocessed);
}

