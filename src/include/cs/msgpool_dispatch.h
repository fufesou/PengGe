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
 * @modified  Sun 2015-12-06 18:23:22 (+0800)
 */

#ifndef _MSGPOOL_DISPATCH
#define _MSGPOOL_DISPATCH

#ifdef __cplusplus
extern "C"
{
#endif

typedef int (CS_CALLBACK *pfunc_msgprocess_t)(char* inmsg, char* outmsg, uint32_t* outmsglen);
typedef int (CS_CALLBACK *pfunc_msgprocess_af_t)(cssock_t handle, void* outmsg);

struct csmsgpool_dispatch
{
    char* prompt;

    struct csmsgpool pool_unprocessed;

    /** This pool can be ignored if immediately sending data is not required after process.
     * 
     * @sa semd_msg
     */
    struct csmsgpool pool_processed;

    /** process_msg This variable must be set to point a function. */
    pfunc_msgprocess_t process_msg;

    /** process_af_msg If send message is not required after process, set process_af_msg to 0;
     */
    pfunc_msgprocess_af_t process_af_msg;
};

/**
 * @brief  csmsgpool_dispatch_init This function should be called first of all.
 *
 * @param pool_dispath
 */
void csmsgpool_dispatch_init(struct csmsgpool_dispatch* pool_dispath);

#ifdef WIN32

/**
 * @brief  csmsgpool_process This is the thread function of send pool.
 *
 * @param pool_dispath This pointer should be casted from (struct cspool_process*);
 *
 * @return   
 */
unsigned int __stdcall csmsgpool_process(void* pool_dispath);

/**
 * @brief  csmsgpool_process_af This is the thread function of recv pool.
 *
 * @param pool_dispath This pointer should be casted from (struct cspool_process*);
 *
 * @return   
 */
unsigned int __stdcall csmsgpool_process_af(void* pool_dispath);

#else

/**
 * @brief  csmsgpool_process This is the thread function of send pool.
 *
 * @param pool_dispath This pointer should be casted from (struct cspool_process*);
 *
 * @return   
 */
void* csmsgpool_process(void* pool_dispath);

/**
 * @brief  csmsgpool_process_af This is the thread function of recv pool.
 *
 * @param pool_dispath This pointer should be casted from (struct cspool_process*);
 *
 * @return   
 */
void* csmsgpool_process_af(void* pool_dispath);
#endif

#ifdef __cplusplus
}
#endif

#endif //MSGPOOL_DISPATCH
