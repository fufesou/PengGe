/**
 * @file lightthread.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Mon 2015-10-26 19:41:18 (+0800)
 */

#include  <stdio.h>
#include  <time.h>
#include  <string.h>

#ifdef WIN32
#include  <windows.h>
#include  <process.h>
#else
#include  <pthread.h>
#include  <unistd.h>
#include  <sys/time.h>
#define THREAD_IDEL_TIMESLICE_MS 20
#endif

#include    "lightthread.h"
#include    "macros.h"
#include    "timespan.h"

#define CS_UNDEFINED	0xffffffff
#define CS_S_OK			0x00000000

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
#endif


#ifndef WIN32
typedef struct{
	void (*proc)(void*);
	void* pargs;
}threadwraper_linux_t;


void* s_thread_process(void* pargs);

#endif

#ifdef __cplusplus
}
#endif


/**
 * @brief The following block describe the windows thread functions.
 *
 */
#ifdef WIN32
#endif


/**
 * @brief The following block describe the unix thread functions.
 *
 */
#ifndef WIN32

void* s_thread_process(void* pargs)
{
	threadwraper_linux_t* pth = (threadwraper_linux_t*)pargs;
	pth->proc(pth->pargs);
	delete [] pth;
	return NULL;
}

int create_thread(void (*proc)(void*), void* pargs)
{
	pthread_t ntid;
	threadwraper_linux_t* pwraper = (threadwraper_linux_t*)malloc(sizeof(threadwraper_linux_t));
	pwraper->proc = proc;
	pwraper->pargs = pargs;
	return pthread_create(&ntid, NULL, s_thread_process, pwraper);
}

void exit_thread(void)
{
	pthread_exit();
}

unsigned int get_pid(void)
{
	return pthread_self();
}

void cs_sleep(unsigned int msec)
{
	if (msec >= 1000) {
		unsigned int s = msec / 1000;
		unsigned int us = msec -  s * 1000;
		sleep(s);

		if (us > 0) {
			usleep(us * 1000);
		}
	} else {
		usleep(msec * 1000);
	}
}


mutex_t create_mutex(void)
{
	mutex_t handle;
	pthread_mutex_init(&handle, NULL);
	return handle;
}

void release_mutex(mutex_t handle)
{
	pthread_mutex_destory(&handle);
}

int lock_mutex(mutex_t handle)
{
	return (pthread_mutex_lock(&handle) == 0) ? 0 : -1;
}

int try_lock_mutex(mutex_t handle, unsigned int msec)
{
	unsigned int usec = msec * 1000;
	timelong_t start;
	int rt;

	if ((rt = pthread_mutex_trylock(&handle)) == EBUSY) {
		reset_timelong(&start);
		while (rt == EBUSY) {
			if (get_span_millisec(&start) > msec) {
				rt = -1;
			} else {
				usleep(2000);
				rt = pthread_mutex_trylock(&handle);
			}
		}
	}

	return rt;
}

void unlock_mutex(mutex_t handle)
{
	pthread_mutex_unlock(&handle);
}


#endif
