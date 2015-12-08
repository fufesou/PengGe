/**
 * @file msgpool.h
 * @brief  This file defines message buffer pool operations.
 * With thread, mutex, semaphore used, This struct csmsgpool can be used for constructing multithread environment.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Sat 2015-11-07 14:52:40 (+0800)
 */

#ifndef  MSGPOOL_H
#define  MSGPOOL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief csmsgpool contains basic members for send receive buffer and thread operations.
 * The members of struct csmsgpool should be set with configuration.
 *
 * @todo write a cofigure parser to set the members
 */
struct csmsgpool {
    int num_thread;
    int len_item;
    int num_item;
    struct array_buf filled_buf;
    struct array_buf empty_buf;

    /** hmutex */
    csmutex_t hmutex;

    /** hthread */
    csthread_t* hthread;

    /** threadexit threadexit is used to make thread exit normally.
     *
     * If threadexit is 1, thread will exit;
     * if threadexit is 0, thread keep on running.
     *
     * @note thread entry must test this value to determine whether exit or not, as the following code shows.
     *
     * @code
     * func()
     * {
     *     while (1) {
     *         if (threadexit) {
     *             break;
     *         }
     *     }
     * }
     * @endcode
     */
    int threadexit;

    /** hsem_filled Be careful, csmsgpool call cssem_wait() and cssem_post() by default.
     */
    cssem_t hsem_filled;

    /** use_sem_in_pool 1 turn on semaphore wait or post, 0 turn off.
     *
     * @note use_sem_in_pool only works in cspool_pulldata and cspool_pushdata.
     * user shoud call semaphore wait and post explicit while calling cspool_pullitem and cspool_pushitem.
     */
    int use_sem_in_pool;

    /** userdata The retain space for user data.
     */
    char userdata[16];
};

/**
 * @brief cspool_init This function must be called before any of pool operation begins.
 * @param pool the operating pool
 * @param itemlen length for each buffer item.
 * @param itemnum number of buffer items.
 * @param threadnum number of threads that the buffer contains.
 * @param userdata if NULL, nothing will be done, else the user data is copied to pool's userdata.
 * @param size_userdata the size of user data.
 * @param pfunc the initial operation that each thread will operate once created.
 * @param pargs this args of pfunc.
 */
void cspool_init(struct csmsgpool* pool, int itemlen, int itemnum, int threadnum, char* userdata, size_t size_userdata, csthread_proc_t proc, void* pargs);

/**
 * @brief  cspool_clear This function will do some clear works such as free memory.
 *
 * @param pool
 */
void cspool_clear(struct csmsgpool* pool);

/**
 * @brief  cspool_pushitem 
 *
 * @param pool
 * @param buf
 * @param item
 *
 * @return   NULL if failed, else success.
 */
char* cspool_pushitem(struct csmsgpool* pool, struct array_buf* buf, char* item);

/**
 * @brief  cspool_pullitem 
 *
 * @param pool
 * @param buf
 *
 * @return   NULL if failed, else success.
 */
char* cspool_pullitem(struct csmsgpool* pool, struct array_buf* buf);

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
int cspool_pushdata(struct csmsgpool* pool, const char* data, int datalen);

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
int cspool_pulldata(struct csmsgpool* pool, char* data, int datalen);

#ifdef __cplusplus
}
#endif


#endif  // MSGPOOL_H
