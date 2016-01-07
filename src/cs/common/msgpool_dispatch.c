/**
 * @file msgpool_dispatch.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-07
 * @modified  Tue 2016-01-05 23:26:56 (+0800)
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

#ifndef _MSC_VER /* *nix */
#include  <semaphore.h>
#endif

#include  <assert.h>
#include  <stdio.h>
#include    "common/jxiot.h"
#include    "common/cstypes.h"
#include    "common/bufarray.h"
#include    "common/list.h"
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "common/msgwrap.h"
#include    "common/processlist.h"
#include    "cs/msgpool.h"
#include    "cs/msgpool_dispatch.h"

#ifdef __cplusplus
extern "C"
{
#endif


#ifdef __cplusplus
}
#endif

void jxmsgpool_dispatch_init(struct jxmsgpool_dispatch* pool_dispatch)
{
    pool_dispatch->prompt = "jxmsgpool_dispatch:";
    pool_dispatch->processlist_head = NULL;

    pool_dispatch->pool_processed.num_thread = 0;
    pool_dispatch->pool_unprocessed.num_thread = 0;
}

#ifdef WIN32
unsigned int __stdcall jxmsgpool_process(void* pool_dispatch)
#else
void* jxmsgpool_process(void* pool_dispatch)
#endif
{
    uint32_t outmsglen;
    char* msgbuf = NULL;
    char* outmsg = NULL;
    pfunc_msgprocess_t pfunc_process = NULL;
    pfunc_msgprocess_af_t pfunc_process_af = NULL;
    struct jxmsgpool_dispatch* msgpool_dispatch = (struct jxmsgpool_dispatch*)pool_dispatch;
    struct jxmsgpool* pool_unproc = &msgpool_dispatch->pool_unprocessed;
    struct jxmsgpool* pool_proced = &msgpool_dispatch->pool_processed;

    printf("%s child recv thread %d created.\n", msgpool_dispatch->prompt, jxthread_getpid());

    if (!jxprocesslist_process_valid(msgpool_dispatch->processlist_head)) {
        printf("%s process_msg is not set, return 0.\n", msgpool_dispatch->prompt);
        return 0;
    }

    while (1)
    {
        jxsem_wait(&pool_unproc->hsem_filled);

        if (pool_unproc->threadexit) {
            break;
        }

        /** The fist char of msgbuf is the mflag. */
        msgbuf = jxpool_pullitem(pool_unproc, &pool_unproc->filled_buf);

        if ((pfunc_process = jxprocesslist_process_get(msgpool_dispatch->processlist_head, *msgbuf)) == NULL) {
            assert(0);
            fprintf(stderr, "message process function should never be NULL.\n");
        }

        printf("recv- thread id: %d, process message: %s.\n", jxthread_getpid(), msgbuf + sizeof(struct jxmsg_header));

        /** If 'process_af_msg' is not valid, outmsg buffer is unused in 'process_msg' */
        if ((pfunc_process_af = jxprocesslist_process_af_get(msgpool_dispatch->processlist_head, *msgbuf))) {
            while ((outmsg = jxpool_pullitem(pool_proced, &pool_proced->empty_buf)) == NULL)
              ;

            outmsglen = pool_proced->len_item;
            *outmsg = *msgbuf;
            pfunc_process(msgbuf + 1, outmsg + 1, &outmsglen);
        } else {
            pfunc_process(msgbuf + 1, NULL, NULL);
        }

        jxpool_pushitem(pool_unproc, &pool_unproc->empty_buf, msgbuf);

        if (pfunc_process_af) {
            if (outmsglen <= 0) {
                jxpool_pushitem(pool_proced, &pool_proced->empty_buf, outmsg);
            } else {
                jxpool_pushitem(pool_proced, &pool_proced->filled_buf, outmsg);
                jxsem_post(&pool_proced->hsem_filled);
            }
        } 
    }

    return 0;
}

#ifdef WIN32
unsigned int __stdcall jxmsgpool_process_af(void* pool_dispatch)
#else
void* jxmsgpool_process_af(void* pool_dispatch)
#endif
{
    char* outmsg = NULL;
    pfunc_msgprocess_af_t pfunc_process_af = NULL;
    struct jxmsgpool_dispatch* msgpool_dispatch = (struct jxmsgpool_dispatch*)pool_dispatch;
    struct jxmsgpool* pool_proced = &msgpool_dispatch->pool_processed;

    printf("%s child send thread %d created.\n", msgpool_dispatch->prompt, jxthread_getpid());

    while (1) {
        jxsem_wait(&pool_proced->hsem_filled);

        if (pool_proced->threadexit) {
            break;
        }

        /** The fist char of outmsg is the mflag. */
        outmsg = jxpool_pullitem(pool_proced, &pool_proced->filled_buf);

        if (!(pfunc_process_af = jxprocesslist_process_af_get(msgpool_dispatch->processlist_head, *outmsg))) {
            printf("%s process_af_msg is not set, ingnore this message.\n", msgpool_dispatch->prompt);
        } else {
            pfunc_process_af(pool_proced->userdata, outmsg + 1);
        }

        jxpool_pushitem(pool_proced, &pool_proced->empty_buf, outmsg);
    }

    return 0;
}
