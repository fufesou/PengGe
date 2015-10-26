/**
 * @file lightthread.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Mon 2015-10-26 19:19:13 (+0800)
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

typedef struct {
	unsigned int errno;
	char errmsg[512];
}thread_error_t;


/**
 * @brief  create_thread 
 *
 * @param proc
 * @param pargs
 *
 * @return   
 */
int create_thread(void (*proc)(void*), void* pargs);
void exit_thread(void);
unsigned int get_pid(void);
void cs_sleep(unsigned int msec);


mutex_t create_mutex(void);
void release_mutex(mutex_t handle);
void lock_mutex(mutex_t handle);
int try_lock_mutex(mutex_t handle, unsigned int msec);
void unlock_mutex(mutex_t handle);

#ifdef __cplusplus
}
#endif

#endif //LIGHTTHREAD_H
