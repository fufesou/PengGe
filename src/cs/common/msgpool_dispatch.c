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

#include  <stdio.h>
#include    "common/jxiot.h"
#include    "common/cstypes.h"
#include    "common/bufarray.h"
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "common/msgwrap.h"
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
    pool_dispatch->process_msg = 0;
    pool_dispatch->process_af_msg = 0;

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
    struct jxmsgpool_dispatch* msgpool_dispatch = (struct jxmsgpool_dispatch*)pool_dispatch;
    struct jxmsgpool* pool_unproc = &msgpool_dispatch->pool_unprocessed;
    struct jxmsgpool* pool_proced = &msgpool_dispatch->pool_processed;

    printf("%s child recv thread %d created.\n", msgpool_dispatch->prompt, jxthread_getpid());

    if (msgpool_dispatch->process_msg == 0) {
        printf("%s process_msg is not set, return 0.\n", msgpool_dispatch->prompt);
        return 0;
    }

    while (1)
    {
        jxsem_wait(&pool_unproc->hsem_filled);

        if (pool_unproc->threadexit) {
            break;
        }

        msgbuf = jxpool_pullitem(pool_unproc, &pool_unproc->filled_buf);

        if (msgpool_dispatch->process_af_msg != 0) {
            while ((outmsg = jxpool_pullitem(pool_proced, &pool_proced->empty_buf)) == NULL)
              ;
        }

        printf("recv- thread id: %d, process message: %s.\n", jxthread_getpid(), msgbuf + sizeof(struct jxmsg_header));

        /** If 'process_af_msg' is not valid, outmsg buffer is unused in 'process_msg' */
        if (msgpool_dispatch->process_af_msg != 0) {
            outmsglen = pool_proced->len_item;
            msgpool_dispatch->process_msg(msgbuf, outmsg, &outmsglen);
        } else {
            msgpool_dispatch->process_msg(msgbuf, NULL, NULL);
        }

        jxpool_pushitem(pool_unproc, &pool_unproc->empty_buf, msgbuf);

        if (msgpool_dispatch->process_af_msg != 0) {
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
    struct jxmsgpool_dispatch* msgpool_dispatch = (struct jxmsgpool_dispatch*)pool_dispatch;
    struct jxmsgpool* pool_proced = &msgpool_dispatch->pool_processed;

    printf("%s child send thread %d created.\n", msgpool_dispatch->prompt, jxthread_getpid());

    if (msgpool_dispatch->process_af_msg == 0) {
        printf("%s process_af_msg is not set, return 0.\n", msgpool_dispatch->prompt);
        return 0;
    }

    while (1) {
        jxsem_wait(&pool_proced->hsem_filled);

        if (pool_proced->threadexit) {
            break;
        }

        outmsg = jxpool_pullitem(pool_proced, &pool_proced->filled_buf);

        msgpool_dispatch->process_af_msg(pool_proced->userdata, outmsg);
        jxsleep(200);

        jxpool_pushitem(pool_proced, &pool_proced->empty_buf, outmsg);
    }

    return 0;
}
