/**
 * @file sendrecv_pool.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 */

#include  <stdio.h>
#include  <windows.h>
#include  <semaphore.h>
#include  <process.h>
#include  <semaphore.h>
#include    "bufarray.h"
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
static void s_create_semaphore(HANDLE* hsem, long count_init, long count_max);
static void s_create_threads(HANDLE** arr_hthreads, int num_thread, ptr_process_sendrecv process_func);


#ifdef __cplusplus
}
#endif


void init_sendrecv_pool(struct sendrecv_pool* pool, int itemlen, int itemnum, int threadnum, SOCKET socket, ptr_process_sendrecv pfunc)
{
#ifdef _CHECK_ARGS
    if (pool == NULL || pfunc == NULL) return 0;
#endif
    pool->len_item = itemlen;
    pool->num_item = itemnum;
    pool->num_thread = threadnum;
    pool->socket = socket;
    pool->process_func = pfunc;

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

    init_buf(&pool->filled_buf, pool->num_item, pool->len_item, init_fillednum);
    pool->len_item = pool->filled_buf.len_item;
    pool->num_item = pool->filled_buf.num_item;

    num_items = pool->filled_buf.get_num_contained_item(&pool->filled_buf);
    init_emptynum = pool->filled_buf.num_item - 1 - num_items;
    init_buf(&pool->empty_buf, pool->num_item, pool->len_item, init_emptynum);

    InitializeCriticalSection(&pool->critical_sec);
    s_create_semaphore(&pool->hsem_filled, 0, pool->filled_buf.num_item - 1);
    s_create_threads(&pool->hthreads, pool->num_thread, pool->process_func);
}

void s_clearpool(struct sendrecv_pool* pool)
{
    int i = 0;

    WaitForMultipleObjects(pool->num_thread, pool->hthreads, 1, INFINITE);
    for (i=0; i<pool->num_thread; ++i) {
        CloseHandle(pool->hthreads[i]);
    }

    CloseHandle(pool->hsem_filled);

    pool->filled_buf.clear_buf(&pool->filled_buf);
    pool->empty_buf.clear_buf(&pool->empty_buf);
    DeleteCriticalSection(&pool->critical_sec);
}

void s_create_semaphore(HANDLE* hsem, long count_init, long count_max)
{
    *hsem = CreateSemaphore(NULL, count_init, count_max, NULL);
    if ((*hsem) == NULL) {
        printf("create semaphore error! errno: %ld\n", GetLastError());
        exit(1);
    }
}

void s_create_threads(HANDLE** arr_hthreads, int num_thread, ptr_process_sendrecv process_func)
{
    int i;
    *arr_hthreads = (HANDLE*)malloc(sizeof(HANDLE) * num_thread);
    for (i=0; i<num_thread; ++i) {
        (*arr_hthreads)[i] = (HANDLE)_beginthreadex(NULL, 0, process_func, NULL, 0, NULL);
        while ((long)(*arr_hthreads)[i] == 1L) {
            printf("create thread error, try again now\n");
            (*arr_hthreads)[i] = (HANDLE)_beginthreadex(NULL, 0, process_func, NULL, 0, NULL);
        }
        if ((*arr_hthreads)[i] == 0) {
            printf("create thread error, errno: %d.\nexit(1)\n", errno);
            exit(1);
        }
        Sleep(20);
    }
}

