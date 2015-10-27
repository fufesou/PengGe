/**
 * @file sendrecv_pool.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  2015-10-28 00:23:43 (+0800)
 */

#ifndef  SENDRECV_POOL_H
#define  SENDRECV_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief sendrecv_pool contains basic members for send receive buffer and thread operations.
 * The members of struct sendrecv_pool should be set with configuration.
 *
 * @todo write a cofigure parser to set the members
 */
struct sendrecv_pool {
    int num_thread;
    int len_item;
    int num_item;
    struct array_buf filled_buf;
    struct array_buf empty_buf;

    cssock_t socket;
    csthread_t* hthread;
    cssem_t hsem_filled;
    csmutex_t hmutex;

    csthread_proc_t proc;

    /**
     * @brief This function should be called after init_sendrecv_pool
     *
     * @see init_sendrecv_pool
     */
    void (*init_pool)(struct sendrecv_pool* pool);
    void (*clear_pool)(struct sendrecv_pool* pool);
};

/**
 * @brief init_sendrecv_pool This function must be called before any of pool operation begins.
 * @param pool
 * @param itemlen
 * @param itemnum
 * @param threadnum
 * @param pfunc
 */
void init_sendrecv_pool(struct sendrecv_pool* pool, int itemlen, int itemnum, int threadnum, cssock_t socket, csthread_proc_t proc);


#ifdef __cplusplus
}
#endif


#endif  // SENDRECV_POOL_H
