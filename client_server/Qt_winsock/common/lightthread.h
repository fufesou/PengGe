/**
 * @file lightthread.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  2015-10-26 23:54:22 (+0800)
 */

#ifndef _LIGHTTHREAD_H
#define _LIGHTTHREAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
	typedef void* mutex_t;
#else
	typedef pthread_mutex_t mutex_t;
#endif


/**
 * @brief  create_thread 
 *
 * @param proc
 * @param pargs
 *
 * @return   
 */
int create_thread(void (*proc)(void*), void* pargs);

/**
 * @brief  exit_thread 
 */
void exit_thread(void);

/**
 * @brief  get_pid 
 *
 * @return   
 */
unsigned int get_pid(void);

/**
 * @brief  cs_sleep 
 *
 * @param msec
 */
void cs_sleep(unsigned int msec);


/**
 * @brief  create_mutex create_mutex will create the mutex and print error message if error occurs.
 *
 * @return   NULL if failed; valid mutex handle if succes.
 */
mutex_t create_mutex(void);

/**
 * @brief  destory_mutex 
 *
 * @param handle
 */
void destory_mutex(mutex_t handle);

/**
 * @brief  lock_mutex 
 *
 * @param handle
 *
 * @return  0 if OK; -1 if failed.
 */
int lock_mutex(mutex_t handle);

/**
 * @brief  try_lock_mutex This function returns zero if a lock on the mutex object referenced by mutex is acquired.
 * Otherwise, an error number is returned to indicate the error.
 *
 * @param handle handle specify the mutex.
 * @param msec msec specify the time span to try to lock the mutex.
 *
 * @return   
 * 1. 0, if success.
 * 2. -1, if timeout.
 * 3. others error code.
 */
int try_lock_mutex(mutex_t handle, unsigned int msec);

/**
 * @brief  unlock_mutex This function unlock the numtex.
 *
 * @param handle handle specify the mutex.
 */
void unlock_mutex(mutex_t handle);

#ifdef __cplusplus
}
#endif

#endif //LIGHTTHREAD_H
