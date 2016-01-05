/**
 * @file lightthread.h
 * @brief  This file defines some basic wrapper functions of thread, mutex and semaphore.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Sun 2015-12-06 18:24:53 (+0800)
 */

#ifndef _LIGHTTHREAD_H
#define _LIGHTTHREAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
    typedef void* jxmutex_t;
    typedef void* jxsem_t;
    typedef void* jxthread_t;
    typedef unsigned int (__stdcall *pthread_proc_t)(void*);

#else
    typedef pthread_mutex_t jxmutex_t;
    typedef sem_t jxsem_t;
    typedef pthread_t jxthread_t;
    typedef void* (JXIOT_CALLBACK *pthread_proc_t)(void*);
#endif


/**************************************************
 **             the thread block                 **
 **************************************************/
/**
 * @brief  jxthread_create 
 *
 * @param proc
 * @param pargs
 * @handle the thread handle
 *
 * @return  On success, returns 0; or, win: return -1, *nix: error number.
 */
JXIOT_API int jxthread_create(pthread_proc_t proc, void* pargs, jxthread_t* handle);

/**
 * @brief  jxthread_exit 
 */
JXIOT_API void jxthread_exit(void);
/**
 * @brief  jxthread_wait_terminate 
 *
 * @param handle
 */
JXIOT_API void jxthread_wait_terminate(jxthread_t handle);

/**
 * @brief  jxthreadN_wait_terminate 
 *
 * @param handle
 * @param count
 */
JXIOT_API void jxthreadN_wait_terminate(jxthread_t* handle, int count);

/**
 * @brief  jxthread_getpid 
 *
 * @return   
 */
JXIOT_API unsigned int jxthread_getpid(void);

/**
 * @brief  jxsleep 
 *
 * @param msec
 */
JXIOT_API void jxsleep(unsigned int msec);


/**************************************************
 **             the mutex block                  **
 **************************************************/
/**
 * @brief  jxmutex_create jxmutex_create will create the mutex and print error message if error occurs.
 *
 * @return   NULL if failed; valid mutex handle if succes.
 */
JXIOT_API jxmutex_t jxmutex_create(void);

/**
 * @brief  jxmutex_destroy 
 *
 * @param handle
 */
JXIOT_API void jxmutex_destroy(jxmutex_t* handle);

/**
 * @brief  jxmutex_lock 
 *
 * @param handle
 *
 * @return  0 if OK; -1 if failed.
 */
JXIOT_API int jxmutex_lock(jxmutex_t* handle);

/**
 * @brief  jxmutex_try_lock This function returns zero if a lock on the mutex object referenced by mutex is acquired.
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
JXIOT_API int jxmutex_try_lock(jxmutex_t* handle, unsigned int msec);

/**
 * @brief  jxmutex_unlock This function unlock the numtex.
 *
 * @param handle handle specify the mutex.
 */
JXIOT_API void jxmutex_unlock(jxmutex_t* handle);


/**************************************************
 **             the semophare block              **
 **************************************************/
JXIOT_API int jxsem_create(int value_init, int value_max, jxsem_t* handle);
#ifdef WIN32
JXIOT_API int jxsem_wait(jxsem_t* handle);
JXIOT_API int jxsem_post(jxsem_t* handle);
JXIOT_API int jxsem_destroy(jxsem_t* handle);
#else
    #define jxsem_wait sem_wait
    #define jxsem_post sem_post
    #define jxsem_destroy sem_destroy
#endif


#ifdef __cplusplus
}
#endif

#endif //LIGHTTHREAD_H
