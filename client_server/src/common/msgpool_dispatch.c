/**
 * @file msgpool_dispatch.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-07
 * @modified  Sat 2015-11-07 15:30:56 (+0800)
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

#include  <stdio.h>
#include  <stdint.h>
#include  <semaphore.h>
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "msgpool.h"
#include    "msgwrap.h"
#include    "msgpool_dispatch.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
}
#endif

void csmsgpool_dispatch_init(struct csmsgpool_dispatch* pool_dispath)
{
	pool_dispath->prompt = "csmsgpool_dispatch:";
	pool_dispath->process_msg = 0;
	pool_dispath->process_af_msg = 0;
	pool_dispath->process_pargs = 0;
}

#ifdef WIN32
unsigned int __stdcall csmsgpool_process(void* pool_dispath)
#else
void* csmsgpool_process(void* pool_dispath)
#endif
{
	int outmsglen;
	char* msgbuf = NULL;
	char* outmsg = NULL;
	struct csmsgpool_dispatch* msgpool_dispatch = (struct csmsgpool_dispatch*)pool_dispath;
    struct csmsgpool* pool_unproc = &msgpool_dispatch->pool_unprocessed;
    struct csmsgpool* pool_proced = &msgpool_dispatch->pool_processed;

    printf("%s child recv thread %d created.\n", msgpool_dispatch->prompt, csthread_getpid());

	if (msgpool_dispatch->process_msg == 0) {
		printf("%s process_msg is not set, return 0.\n", msgpool_dispatch->prompt);
		return 0;
	}

    while (1)
    {
        cssem_wait(&pool_unproc->hsem_filled);

        if (pool_unproc->threadexit) {
            break;
        }

        msgbuf = cspool_pullitem(pool_unproc, &pool_unproc->filled_buf);

		if (msgpool_dispatch->process_af_msg != 0) {
            while ((outmsg = cspool_pullitem(pool_proced, &pool_proced->empty_buf)) == NULL)
			  ;
		}

        printf("recv- thread id: %d, process message: %s.\n", csthread_getpid(), msgbuf + sizeof(struct csmsg_header));

        outmsglen = pool_proced->len_item;
        msgpool_dispatch->process_msg(msgbuf, outmsg, &outmsglen, msgpool_dispatch->process_pargs);

        cspool_pushitem(pool_unproc, &pool_unproc->empty_buf, msgbuf);

		if (msgpool_dispatch->process_af_msg != 0) {
			if (outmsglen <= 0) {
                cspool_pushitem(pool_proced, &pool_proced->empty_buf, outmsg);
			} else {
                cspool_pushitem(pool_proced, &pool_proced->filled_buf, outmsg);
                cssem_post(&pool_proced->hsem_filled);
			}
		} 
	}

    return 0;
}

#ifdef WIN32
unsigned int __stdcall csmsgpool_process_af(void* pool_dispath)
#else
void* csmsgpool_process_af(void* pool_dispath)
#endif
{
    char* outmsg = NULL;
    struct csmsgpool_dispatch* msgpool_dispatch = (struct csmsgpool_dispatch*)pool_dispath;
    struct csmsgpool* pool_proced = &msgpool_dispatch->pool_processed;

    printf("%s child send thread %d created.\n", msgpool_dispatch->prompt, csthread_getpid());

	if (msgpool_dispatch->process_af_msg == 0) {
		printf("%s process_af_msg is not set, return 0.\n", msgpool_dispatch->prompt);
		return 0;
	}

    while (1) {
        cssem_wait(&pool_proced->hsem_filled);

        if (pool_proced->threadexit) {
            break;
        }

        outmsg = cspool_pullitem(pool_proced, &pool_proced->filled_buf);

        msgpool_dispatch->process_af_msg(pool_proced->socket, outmsg);
        cssleep(200);

        cspool_pushitem(pool_proced, &pool_proced->empty_buf, outmsg);
    }

    return 0;
}
