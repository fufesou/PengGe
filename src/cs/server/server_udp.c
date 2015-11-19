/**
 * @file csserver.c
 * @brief This file process server udp message send & recv. For now a lot config data is hard coded, reimplementation must be done when config parser complete.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 * @modified  Wed 2015-11-18 18:02:09 (+0800)
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
#include  <semaphore.h>
#include    "config_macros.h"
#include	"macros.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "msgpool.h"
#include    "sock_wrap.h"
#include    "msgwrap.h"
#include    "server.h"
#include    "msgpool.h"
#include    "msgpool_dispatch.h"
#include    "server_msgdispatch.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief s_msgpool_dispatch This variable is used to manage server send and recv pool.
 */
static struct csmsgpool_dispatch s_msgpool_dispatch;

static void s_init_msgpool_dispatch(struct csserver* serv);
static void s_clear_msgpool_dispatch(void);

#ifdef __cplusplus
}
#endif

void csserver_udp(struct csserver* serv)
{
    char* buf = NULL;
	int numbytes = 0;
    struct csmsgpool* recvpool = &s_msgpool_dispatch.pool_unprocessed;

    s_init_msgpool_dispatch(serv);

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

    s_clear_msgpool_dispatch();
}

void s_init_msgpool_dispatch(struct csserver* serv)
{	
	csmsgpool_dispatch_init(&s_msgpool_dispatch);

    s_msgpool_dispatch.prompt = "server msgpool_dispatch:";
	s_msgpool_dispatch.process_msg = csserver_process_msg;
    s_msgpool_dispatch.process_af_msg = csserver_send;

    cspool_init(
                &s_msgpool_dispatch.pool_unprocessed,			/** struct csmsgpool* pool	*/
                MAX_MSG_LEN + sizeof(struct csmsg_header),		/** int itemlen 			*/
                SERVER_POOL_NUM_ITEM,							/** int itemnum 			*/
                NUM_THREAD,										/** int threadnum			*/
                serv->hsock,									/** cssock_t socket 		*/
                csmsgpool_process,								/** csthread_proc_t proc 	*/
                (void*)&s_msgpool_dispatch);					/** void* pargs 			*/

    cspool_init(
                &s_msgpool_dispatch.pool_processed,
                MAX_MSG_LEN + sizeof(struct csmsg_header),
                SERVER_POOL_NUM_ITEM,
                NUM_THREAD,
                serv->hsock,
                csmsgpool_process_af,
                (void*)&s_msgpool_dispatch);
}

void s_clear_msgpool_dispatch()
{
    cspool_clear(&s_msgpool_dispatch.pool_unprocessed);
    cspool_clear(&s_msgpool_dispatch.pool_processed);
}
