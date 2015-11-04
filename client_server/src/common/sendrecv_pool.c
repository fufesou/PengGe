/**
 * @file cssendrecv_pool.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Sat 2015-10-31 15:43:58 (+0800)
 */

#ifdef WIN32
#include  <Windows.h>
#else
#include  <sys/socket.h>
#endif

#include  <stdio.h>
#include  <malloc.h>
#include  <stdint.h>
#include  <string.h>
#include  <semaphore.h>
#include    "macros.h"
#include    "utility_wrap.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "sendrecv_pool.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  s_initpool This function will initialize buffers, create semaphore and threads.
 *
 * @param pool The send or receive pool to be initialized.
 *
 * @note Correct sequence for initialize is required here.
 * 1. buffers
 * 2. critical section
 * 3. semaphare
 * 4. threads
 */
static void s_initpool(struct cssendrecv_pool* pool);

/**
 * @brief  s_clearpool This function will do some clear works such as free memory.

 *
 * @param pool The send or receive pool to clear.
 */
static void s_clearpool(struct cssendrecv_pool* pool);


#ifdef __cplusplus
}
#endif


void cspool_init(struct cssendrecv_pool* pool, int itemlen, int itemnum, int threadnum, cssock_t socket, csthread_proc_t proc)
{
#ifdef _CHECK_ARGS
    if (pool == NULL || pfunc == NULL) return 0;
#endif
    pool->len_item = itemlen;
    pool->num_item = itemnum;
    pool->num_thread = threadnum;
    pool->socket = socket;
    pool->proc = proc;

	s_initpool(pool);
}

void cspool_clear(struct cssendrecv_pool* pool)
{
	s_clearpool(pool);
}

void s_initpool(struct cssendrecv_pool* pool)
{
    const int init_fillednum = 0;
    int num_items;
    int init_emptynum;
	int i;

    init_buf(&pool->filled_buf, pool->num_item, pool->len_item, init_fillednum);
    pool->len_item = pool->filled_buf.len_item;
    pool->num_item = pool->filled_buf.num_item;

    num_items = pool->filled_buf.get_num_contained_item(&pool->filled_buf);
    init_emptynum = pool->filled_buf.num_item - 1 - num_items;
    init_buf(&pool->empty_buf, pool->num_item, pool->len_item, init_emptynum);

    pool->hmutex = csmutex_create();
    cssem_create(0, pool->filled_buf.num_item - 1, &pool->hsem_filled);
    pool->use_sem_in_pool = 0;

    pool->hthread = (csthread_t*)malloc(sizeof(csthread_t) * pool->num_thread);
	for (i=0; i<pool->num_thread; ++i) {
		csthread_create(pool->proc, NULL, pool->hthread + i);
		cssleep(20);
	}
}

void s_clearpool(struct cssendrecv_pool* pool)
{
	csthreadN_wait_terminate(pool->hthread, pool->num_thread);
	cssem_destroy(&pool->hsem_filled);

    pool->filled_buf.clear_buf(&pool->filled_buf);
    pool->empty_buf.clear_buf(&pool->empty_buf);
	csmutex_destroy(pool->hmutex);
}

char* cspool_pushitem(struct cssendrecv_pool* pool, struct array_buf* buf, char* item)
{
    if (pool->use_sem_in_pool && buf == (&pool->filled_buf)) {
        if (cssem_post(&pool->hsem_filled) != 0) {
            return NULL;
        }
    }

    csmutex_lock(pool->hmutex);
    item = buf->push_item(buf, item);
    csmutex_unlock(pool->hmutex);
    return item;
}

char* cspool_pullitem(struct cssendrecv_pool* pool, struct array_buf* buf)
{
    char* item;

    if (pool->use_sem_in_pool && buf == (&pool->filled_buf)) {
        if (cssem_wait(&pool->hsem_filled) != 0) {
            return NULL;
        }
    }

    csmutex_lock(pool->hmutex);
    item = buf->pull_item(buf);
    csmutex_unlock(pool->hmutex);
    return item;
}

int cspool_pushdata(struct cssendrecv_pool* pool, const char* data, int datalen)
{
    char* poolbuf = NULL;

    csmutex_lock(pool->hmutex);
    poolbuf = pool->empty_buf.pull_item(&pool->empty_buf);
    csmutex_unlock(pool->hmutex);

    if (poolbuf == NULL) {
        return 1;
    }

    if (cs_memcpy(poolbuf, pool->len_item, data, datalen) != 0)
    {
        csmutex_lock(pool->hmutex);
        pool->empty_buf.push_item(&pool->empty_buf, poolbuf);
        csmutex_unlock(pool->hmutex);
        return -1;
    }

    csmutex_lock(pool->hmutex);
    pool->filled_buf.push_item(&pool->filled_buf, poolbuf);
    csmutex_unlock(pool->hmutex);
    if (pool->use_sem_in_pool) {
        /*
         * @brief error handle block should be added here.
         */
        cssem_post(&pool->hsem_filled);
    }

    return 0;
}

int cspool_pulldata(struct cssendrecv_pool* pool, char* data, int datalen)
{
    char* bufitem = NULL;

    if (pool->use_sem_in_pool) {
        if (cssem_wait(&pool->hsem_filled) != 0) {
            return 2;
        }
    }
    csmutex_lock(pool->hmutex);
    bufitem = pool->filled_buf.pull_item(&pool->filled_buf);
    csmutex_unlock(pool->hmutex);

    if (bufitem == NULL) {
        return 1;
    }

    if (cs_memcpy(data, datalen, bufitem, strlen(bufitem) + 1) != 0)
    {
        csmutex_lock(pool->hmutex);
        pool->filled_buf.push_item(&pool->filled_buf, bufitem);
        csmutex_unlock(pool->hmutex);
        return -1;
    }

    csmutex_lock(pool->hmutex);
    pool->empty_buf.push_item(&pool->empty_buf, bufitem);
    csmutex_unlock(pool->hmutex);

    return 0;
}