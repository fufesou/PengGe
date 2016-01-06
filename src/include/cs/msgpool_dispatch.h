/**
 * @file msgpool_dispatch.h
 * @brief  This file is the process register module. 
 *
 * It simply process message in unprocessed pool(message that received from peer port) 
 * and processed pool(message prcessed).
 *
 * 1. When prcess recieved message. 
 * It firstly pull message from the unprocessed pool, then process message by calling designated process function,
 * and at last push the message to processed pool.
 * 2. When prcess processed message. It firstly pull message from the processed pool,
 * then call process_af(if this function pointer is not NULL).
 *
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-07
 * @modified  Wed 2016-01-06 21:10:45 (+0800)
 */

#ifndef _MSGPOOL_DISPATCH
#define _MSGPOOL_DISPATCH

#ifdef __cplusplus
extern "C"
{
#endif

struct jxmsgpool_dispatch
{
    char* prompt;

    struct jxmsgpool pool_unprocessed;

    /** This pool can be ignored if immediately sending data is not required after process.
     * 
     * @sa semd_msg
     */
    struct jxmsgpool pool_processed;

    /**
     * @brief  processlist_head the items of list_process_head contain mflag and corresponding process functions.
     *
     * @sa struct list_process_t
     */
    struct list_head* processlist_head;
};

/**
 * @brief  jxmsgpool_dispatch_init This function should be called first of all.
 *
 * @param pool_dispatch
 */
void jxmsgpool_dispatch_init(struct jxmsgpool_dispatch* pool_dispatch);

#ifdef WIN32

/**
 * @brief  jxmsgpool_process This is the thread function of send pool.
 *
 * @param pool_dispatch This pointer should be casted from (struct jxpool_process*);
 *
 * @return   
 */
unsigned int __stdcall jxmsgpool_process(void* pool_dispatch);

/**
 * @brief  jxmsgpool_process_af This is the thread function of recv pool.
 *
 * @param pool_dispatch This pointer should be casted from (struct jxpool_process*);
 *
 * @return   
 */
unsigned int __stdcall jxmsgpool_process_af(void* pool_dispatch);

#else

/**
 * @brief  jxmsgpool_process This is the thread function of send pool.
 *
 * @param pool_dispatch This pointer should be casted from (struct jxpool_process*);
 *
 * @return   
 */
void* jxmsgpool_process(void* pool_dispatch);

/**
 * @brief  jxmsgpool_process_af This is the thread function of recv pool.
 *
 * @param pool_dispatch This pointer should be casted from (struct jxpool_process*);
 *
 * @return   
 */
void* jxmsgpool_process_af(void* pool_dispatch);
#endif

#ifdef __cplusplus
}
#endif

#endif //MSGPOOL_DISPATCH
