/**
 * @file sendrecv_pool.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  2015-10-28 00:55:47 (+0800)
 */

#ifdef WIN32
#include  <Windows.h>
#endif

#include  <stdio.h>
#include  <malloc.h>
#include  <stdint.h>
#include  <semaphore.h>
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
 */
static void s_initpool(struct sendrecv_pool* pool);
/**
 * @brief  s_clearpool This function will do some clear works such as free memory.
 *
 * @param pool The send or receive pool to clear.
 */
static void s_clearpool(struct sendrecv_pool* pool);


#ifdef __cplusplus
}
#endif


void init_sendrecv_pool(struct sendrecv_pool* pool, int itemlen, int itemnum, int threadnum, cssock_t socket, csthread_proc_t proc)
{
#ifdef _CHECK_ARGS
    if (pool == NULL || pfunc == NULL) return 0;
#endif
    pool->len_item = itemlen;
    pool->num_item = itemnum;
    pool->num_thread = threadnum;
    pool->socket = socket;
    pool->proc = proc;

    pool->init_pool = s_initpool;
    pool->clear_pool = s_clearpool;
}

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
void s_initpool(struct sendrecv_pool* pool)
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

    pool->hthread = (csthread_t*)malloc(sizeof(csthread_t) * pool->num_thread);
	for (i=0; i<pool->num_thread; ++i) {
		csthread_create(pool->proc, NULL, pool->hthread + i);
		cssleep(20);
	}
}

void s_clearpool(struct sendrecv_pool* pool)
{
	csthreadN_wait_terminate(pool->hthread, pool->num_thread);
	cssem_destroy(&pool->hsem_filled);

    pool->filled_buf.clear_buf(&pool->filled_buf);
    pool->empty_buf.clear_buf(&pool->empty_buf);
	csmutex_destroy(pool->hmutex);
}

