/**
 * @file sendrecv_pool.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  周四 2015-11-05 10:14:57 中国标准时间
 */

#ifndef  SENDRECV_POOL_H
#define  SENDRECV_POOL_H

#include    "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief cssendrecv_pool contains basic members for send receive buffer and thread operations.
 * The members of struct cssendrecv_pool should be set with configuration.
 *
 * @todo write a cofigure parser to set the members
 */
struct cssendrecv_pool {
    int num_thread;
    int len_item;
    int num_item;
    struct array_buf filled_buf;
    struct array_buf empty_buf;

    /**
     * @brief socket
     */
    cssock_t socket;

    /**
     * @brief hmutex
     */
    csmutex_t hmutex;

    /**
     * @brief hthread
     */
    csthread_t* hthread;

    /**
     * @brief hsem_filled Be careful, cssendrecv_pool call cssem_wait() and cssem_post() by default.
     */
    cssem_t hsem_filled;

    /**
     * @brief use_sem_in_pool 1 turn on semaphore wait or post, 0 turn off.
     *
     * @note use_sem_in_pool only works in cspool_pulldata and cspool_pushdata.
     * user shoud call semaphore wait and post explicit while calling cspool_pullitem and cspool_pushitem.
     */
    int use_sem_in_pool;

    /**
     * @brief proc
     */
    csthread_proc_t proc;
};

/**
 * @brief cspool_init This function must be called before any of pool operation begins.
 * @param pool the operating bool
 * @param itemlen length for each buffer item.
 * @param itemnum number of buffer items.
 * @param threadnum number of threads that the buffer contains.
 * @param pfunc the initial operation that each thread will operate once created.
 */
void cspool_init(struct cssendrecv_pool* pool, int itemlen, int itemnum, int threadnum, cssock_t socket, csthread_proc_t proc);

/**
 * @brief  cspool_clear This function will do some clear works such as free memory.
 *
 * @param pool
 */
void cspool_clear(struct cssendrecv_pool* pool);

/**
 * @brief  cspool_pushitem 
 *
 * @param pool
 * @param buf
 * @param item
 *
 * @return   NULL if failed, else success.
 */
char* cspool_pushitem(struct cssendrecv_pool* pool, struct array_buf* buf, char* item);

/**
 * @brief  cspool_pullitem 
 *
 * @param pool
 * @param buf
 *
 * @return   NULL if failed, else success.
 */
char* cspool_pullitem(struct cssendrecv_pool* pool, struct array_buf* buf);

/**
 * @brief  cspool_pushdata This function push one buffer item data to pool. This procedure can be splitted into two steps:
 * 1. take out one item from the empty buffer.
 * 2. copy data from 'data' to the buffer item. 
 * 3. push buffer item into the filled buffer.
 *
 * @param pool the operating pool.
 * @param data
 * @param datalen
 *
 * @return   
 * 0 if success. There is at least one empty buffer.
 * 1 if fail. There is no empty buffer.
 * -1 if fail. copy data error.
 */
int cspool_pushdata(struct cssendrecv_pool* pool, const char* data, int datalen);

/**
 * @brief  cspool_pulldata This function pull one buffer item data from filled buffer and move the item into the empty buffer.
 *
 * @param pool
 * @param data
 * @param datalen 
 *
 * @return   
 * 0 if success.
 * 1 if there is no filled buffer.
 * 2 if wait semaphore failed.
 * -1 if size of data is not large enough.
 */
int cspool_pulldata(struct cssendrecv_pool* pool, char* data, int datalen);

#ifdef __cplusplus
}
#endif


#endif  // SENDRECV_POOL_H
