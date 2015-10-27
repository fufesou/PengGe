/**
 * @file lightthread.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Tue 2015-10-27 19:45:37 (+0800)
 */

#ifndef _LIGHTTHREAD_H
#define _LIGHTTHREAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
	typedef void* csmutex_t;
	typedef void* cssem_t;
	typedef uintptr_t csthread_t;
#else
	typedef pthread_mutex_t csmutex_t;
	typedef sem_t cssem_t;
	typedef pthread_t csthread_t;
#endif


/**************************************************
 **             the thread block                 **
 **************************************************/
/**
 * @brief  create_thread 
 *
 * @param proc
 * @param pargs
 * @handle the thread handle
 *
 * @return   
 */
int create_thread(void (*proc)(void*), void* pargs, csthread_t* handle);

/**
 * @brief  exit_thread 
 */
void exit_thread(void);
/**
 * @brief  wait_thread_terminate 
 *
 * @param handle
 */
void wait_thread_terminate(csthread_t handle);

/**
 * @brief  wait_threadN_terminate 
 *
 * @param handle
 * @param count
 */
void wait_threadN_terminate(csthread_t* handle, int count);

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


/**************************************************
 **             the mutex block                  **
 **************************************************/
/**
 * @brief  create_mutex create_mutex will create the mutex and print error message if error occurs.
 *
 * @return   NULL if failed; valid mutex handle if succes.
 */
csmutex_t create_mutex(void);

/**
 * @brief  destory_mutex 
 *
 * @param handle
 */
void destory_mutex(csmutex_t handle);

/**
 * @brief  lock_mutex 
 *
 * @param handle
 *
 * @return  0 if OK; -1 if failed.
 */
int lock_mutex(csmutex_t handle);

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
int try_lock_mutex(csmutex_t handle, unsigned int msec);

/**
 * @brief  unlock_mutex This function unlock the numtex.
 *
 * @param handle handle specify the mutex.
 */
void unlock_mutex(csmutex_t handle);


/**************************************************
 **             the semophare block              **
 **************************************************/
int cssem_create(int value_init, int value_max, cssem_t* handle);
#ifdef WIN32
int cssem_wait(cssem_t* handle);
int cssem_post(cssem_t* handle);
int cssem_destroy(cssem_t* handle);
#else
	#define cssem_wait sem_wait
	#define cssem_post sem_post
	#define cssem_destroy sem_destroy
#endif


#ifdef __cplusplus
}
#endif

#endif //LIGHTTHREAD_H
