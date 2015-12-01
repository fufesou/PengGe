/**
 * @file lightthread.h
 * @brief  This file defines some basic wrapper functions of thread, mutex and semaphore.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-26
 * @modified  2015-10-28 00:56:06 (+0800)
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
    typedef void* csthread_t;
	typedef unsigned int (__stdcall *csthread_proc_t)(void*);

#else
	typedef pthread_mutex_t csmutex_t;
	typedef sem_t cssem_t;
	typedef pthread_t csthread_t;
	typedef void* (*csthread_proc_t)(void*);
#endif


/**************************************************
 **             the thread block                 **
 **************************************************/
/**
 * @brief  csthread_create 
 *
 * @param proc
 * @param pargs
 * @handle the thread handle
 *
 * @return   
 */
int csthread_create(csthread_proc_t proc, void* pargs, csthread_t* handle);

/**
 * @brief  csthread_exit 
 */
void csthread_exit(void);
/**
 * @brief  csthread_wait_terminate 
 *
 * @param handle
 */
void csthread_wait_terminate(csthread_t handle);

/**
 * @brief  csthreadN_wait_terminate 
 *
 * @param handle
 * @param count
 */
void csthreadN_wait_terminate(csthread_t* handle, int count);

/**
 * @brief  csthread_getpid 
 *
 * @return   
 */
unsigned int csthread_getpid(void);

/**
 * @brief  cssleep 
 *
 * @param msec
 */
void cssleep(unsigned int msec);


/**************************************************
 **             the mutex block                  **
 **************************************************/
/**
 * @brief  csmutex_create csmutex_create will create the mutex and print error message if error occurs.
 *
 * @return   NULL if failed; valid mutex handle if succes.
 */
csmutex_t csmutex_create(void);

/**
 * @brief  csmutex_destroy 
 *
 * @param handle
 */
void csmutex_destroy(csmutex_t* handle);

/**
 * @brief  csmutex_lock 
 *
 * @param handle
 *
 * @return  0 if OK; -1 if failed.
 */
int csmutex_lock(csmutex_t* handle);

/**
 * @brief  csmutex_try_lock This function returns zero if a lock on the mutex object referenced by mutex is acquired.
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
int csmutex_try_lock(csmutex_t* handle, unsigned int msec);

/**
 * @brief  csmutex_unlock This function unlock the numtex.
 *
 * @param handle handle specify the mutex.
 */
void csmutex_unlock(csmutex_t* handle);


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
