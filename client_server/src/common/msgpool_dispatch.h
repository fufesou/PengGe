/**
 * @file msgpool_dispatch.h
 * @brief  This file simply process message in send and recv pool. 
 * 1. When prcess recv message. It firstly pull message from the recv pool, then process message by calling designated process function, and at last push the message to send pool.
 * 2. When prcess send message. It firstly pull message from the send pool, then send message to the peer port.
 *
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-07
 * @modified  Sat 2015-11-07 15:30:01 (+0800)
 */

#ifndef _MSGPOOL_DISPATCH
#define _MSGPOOL_DISPATCH

#ifdef __cplusplus
extern "C"
{
#endif

typedef void csmsg_process_t(char* inmsg, char* outmsg, int* outmsglen, void* pargs);
typedef void csmsg_process_af_t(cssock_t handle, void* outmsg);

struct csmsgpool_dispatch
{
	char* prompt;

    struct csmsgpool pool_unprocessed;

	/**
	 * @brief  This pool can be ignored if immediately sending data is not required after process.
	 * 
	 * @sa semd_msg
	 */
	struct csmsgpool pool_processed;

	/**
	 * @brief  process_msg This variable must be set to point a function.
	 */
	csmsg_process_t* process_msg;

	/**
	 * @brief process_af_msg If send message is not required after process, set process_af_msg to 0;
	 */
	csmsg_process_af_t* process_af_msg;

	void* process_pargs;
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
