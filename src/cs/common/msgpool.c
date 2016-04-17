/**
 * @file msgpool.c
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Sun 2015-12-06 18:13:39 (+0800)
 */

#ifdef WIN32
#include  <Windows.h>
#else
#include  <sys/socket.h>
#endif

#ifndef _MSC_VER /* *nix */
#include  <semaphore.h>
#endif

#include  <stdio.h>
#include  <malloc.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/utility_wrap.h"
#include    "common/bufarray.h"
#include    "common/sock_types.h"
#include    "common/lightthread.h"
#include    "cs/msgpool.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  s_initpool This function will initialize buffers, create semaphore and threads.
 *
 * @param pool The send or receive pool to be initialized.
 * @proc  the function entry of thread.
 * @pargs the parameter of proc.
 *
 * @note Correct sequence for initialize is required here.
 * 1. buffers
 * 2. critical section
 * 3. semaphare
 * 4. threads
 */
static void s_initpool(struct jxmsgpool* pool, pthread_proc_t proc, void* pargs);

/**
 * @brief  s_clearpool This function will do some clear works such as free memory.
 *
 * @param pool The send or receive pool to clear.
 */
static void s_clearpool(struct jxmsgpool* pool);


#ifdef __cplusplus
}
#endif


void jxpool_init(struct jxmsgpool* pool, int itemlen, int itemnum, int threadnum, char* userdata, size_t size_userdata, pthread_proc_t proc, void* pargs)
{
#ifdef _CHECK_ARGS
    if (pool == NULL || pfunc == NULL) return 0;
#endif
    pool->len_item = itemlen;
    pool->num_item = itemnum;
    pool->num_thread = threadnum;
    pool->threadexit = 0;

    jxmemcpy(pool->userdata, sizeof(pool->userdata), userdata, size_userdata);

    s_initpool(pool, proc, pargs);
}

void jxpool_clear(struct jxmsgpool* pool)
{
    s_clearpool(pool);
}

void s_initpool(struct jxmsgpool* pool, pthread_proc_t proc, void* pargs)
{
    const int init_fillednum = 0;
    int num_items;
    int init_emptynum;
    int i;

    init_buf(&pool->filled_buf, pool->num_item, pool->len_item, init_fillednum);
    pool->len_item = pool->filled_buf.len_item;
    pool->num_item = pool->filled_buf.num_item;

    num_items = get_num_contained_items(&pool->filled_buf);
    init_emptynum = pool->filled_buf.num_item - 1 - num_items;
    init_buf(&pool->empty_buf, pool->num_item, pool->len_item, init_emptynum);

    pool->hmutex = jxmutex_create();
    jxsem_create(0, pool->filled_buf.num_item - 1, &pool->hsem_filled);
    pool->use_sem_in_pool = 1;

    pool->hthread = (jxthread_t*)malloc(sizeof(jxthread_t) * pool->num_thread);
    for (i=0; i<pool->num_thread; ++i) {
        jxthread_create(proc, pargs, pool->hthread + i);
        jxsleep(20);
    }
}

void s_clearpool(struct jxmsgpool* pool)
{
    int i = 0;

    /**
      * @brief make sure all threads will not block for semaphore.
      */
    pool->threadexit = 1;
    for (; i<pool->num_thread; ++i) {
        jxsem_post(&pool->hsem_filled);
    }

    jxthreadN_wait_terminate(pool->hthread, pool->num_thread);
    jxsem_destroy(&pool->hsem_filled);

    clear_buf(&pool->filled_buf);
    clear_buf(&pool->empty_buf);
    jxmutex_destroy(&pool->hmutex);
}

char* jxpool_pushitem(struct jxmsgpool* pool, struct array_buf* buf, char* item)
{
    jxmutex_lock(&pool->hmutex);
    item = push_item(buf, item);
    jxmutex_unlock(&pool->hmutex);

    return item;
}

char* jxpool_pullitem(struct jxmsgpool* pool, struct array_buf* buf)
{
    char* item;

    jxmutex_lock(&pool->hmutex);
    item = pull_item(buf);
    jxmutex_unlock(&pool->hmutex);
    return item;
}

int jxpool_pushdata(struct jxmsgpool* pool, const char* data, int datalen)
{
    char* poolbuf = NULL;

    jxmutex_lock(&pool->hmutex);
    poolbuf = pull_item(&pool->empty_buf);
    jxmutex_unlock(&pool->hmutex);

    if (poolbuf == NULL) {
        return 1;
    }

    if (jxmemcpy(poolbuf, pool->len_item, data, datalen) != 0)
    {
        jxmutex_lock(&pool->hmutex);
        push_item(&pool->empty_buf, poolbuf);
        jxmutex_unlock(&pool->hmutex);
        return -1;
    }

    jxmutex_lock(&pool->hmutex);
    push_item(&pool->filled_buf, poolbuf);
    jxmutex_unlock(&pool->hmutex);
    if (pool->use_sem_in_pool) {
        /*
         * @brief error handle block should be added here.
         */
        jxsem_post(&pool->hsem_filled);
    }

    return 0;
}

int jxpool_pulldata(struct jxmsgpool* pool, char* data, int datalen)
{
    char* bufitem = NULL;

    if (pool->use_sem_in_pool) {
        if (jxsem_wait(&pool->hsem_filled) != 0) {
            return 2;
        }
    }
    jxmutex_lock(&pool->hmutex);
    bufitem = pull_item(&pool->filled_buf);
    jxmutex_unlock(&pool->hmutex);

    if (bufitem == NULL) {
        return 1;
    }

    if (jxmemcpy(data, datalen, bufitem, strlen(bufitem) + 1) != 0)
    {
        jxmutex_lock(&pool->hmutex);
        push_item(&pool->filled_buf, bufitem);
        jxmutex_unlock(&pool->hmutex);
        return -1;
    }

    jxmutex_lock(&pool->hmutex);
    push_item(&pool->empty_buf, bufitem);
    jxmutex_unlock(&pool->hmutex);

    return 0;
}
