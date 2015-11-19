/**
 * @file client_udp.c
 * @brief  This file prcess client udp message sendrecv.
 *
 * For now a lot config data is hard coded, reimplementation must be done when config parser complete.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-07
 * @modified  Wed 2015-11-18 23:16:44 (+0800)
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

#include  <assert.h>
#include  <stdlib.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <string.h>
#include  <semaphore.h>
#include    "config_macros.h"
#include	"macros.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "utility_wrap.h"
#include    "msgpool.h"
#include    "sock_wrap.h"
#include    "msgwrap.h"
#include    "msgpool_dispatch.h"
#include    "client.h"
#include    "account.h"


#ifdef __cplusplus
extern "C"
{
#endif

extern const char* g_exit;

/** s_msgpool_dispatch This variable is used to manage client send and recv pool. */
static struct csmsgpool_dispatch s_msgpool_dispatch;

static void s_dispatch_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static void s_init_msgpool_dispatch(struct csclient* cli);
static void s_clear_msgpool_dispatch(void);
static void s_msgpool_append(char* data, ssize_t numbytes);

void client_udp_init(struct csclient* cli);
void client_udp_clear(void);

static int s_inited = 0;
static int s_cleared = 0;

#ifdef __cplusplus
}
#endif

void client_udp_init(struct csclient* cli)
{
    if (!s_inited) {
        s_inited = 1;

        s_init_msgpool_dispatch(cli);
        printf("%s enter login session.\n", cli->prompt);
    }
}

void client_udp_clear()
{
    if (!s_cleared) {
        s_cleared = 1;
        s_clear_msgpool_dispatch();
    }
}

void csclient_udp(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    client_udp_init(cli);

    csclient_connect(cli, servaddr, addrlen);
    while (fgets(cli->sendbuf, sizeof(cli->sendbuf), fp) != NULL) {
        cli->sendbuf[strlen(cli->sendbuf) - 1] = '\0';

        if (strncmp(cli->sendbuf, g_exit, strlen(g_exit) + 1) == 0) {
            printf("%s exit.\n", cli->prompt);
            break;
        }

		csclient_udp_once(cli, servaddr, addrlen);
    }

    client_udp_clear();
}

void csclient_udp_once(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen)
{
    ssize_t numbytes = csclient_sendrecv(cli, servaddr, addrlen);
	if (numbytes > 0) {
		s_msgpool_append(cli->recvbuf, numbytes);
	}
}


/**
 * @brief  s_dispatch_react The income message from server 
 * will be handle by the coressponding 'react' method in this function.
 *
 * @param inmsg The format of inmsg is:
 * ----------------------------------------------------------------------------------------------
 * | struct csmsg_header | process id(uint32_t) | user id(uint32_t) | process data(char*) | ... |
 * ----------------------------------------------------------------------------------------------
 * @param outmsg
 * @param outmsglen
 */
void s_dispatch_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    uint32_t id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));
	am_method_get(id_process)->react(inmsg + sizeof(struct csmsg_header), outmsg, outmsglen);
}

void s_init_msgpool_dispatch(struct csclient* cli)
{	
	const int threadnum = 1; 

	csmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "client msgpool_dispatch:";
	s_msgpool_dispatch.process_msg = s_dispatch_react;
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
