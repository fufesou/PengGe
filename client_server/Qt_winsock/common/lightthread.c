/**
 * @file lightthread.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  2015-10-26 23:27:26 (+0800)
 */

#include  <time.h>
#include  <stdio.h>
#include  <malloc.h>
#include  <string.h>

#ifdef WIN32
#include  <Windows.h>
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
int create_thread(void (*proc)(void*), void* pargs)
{
    return _beginthread(proc, 0, pargs);
}

void exit_thread(void)
{
	_endthread();
}

unsigned int get_pid(void)
{
	return GetCurrentThreadId();
}

void cs_sleep(unsigned int msec)
{
	Sleep(msec);
}

mutex_t create_mutex(void)
{
	mutex_t handle;
	if ((handle = CreateMutex(NULL, FALSE, NULL)) == NULL) {
        fprintf(stderr, "create thread fail, error code: %ld\n", GetLastError());
	}
	return handle;
}

void destory_mutex(mutex_t handle)
{
	if (handle) {
		CloseHandle(handle);
	}
}

int lock_mutex(mutex_t handle)
{
	if (handle && WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0) {
		return 0;
	}
	return -1;
}

int try_lock_mutex(mutex_t handle, unsigned int msec)
{
	if (handle && WaitForSingleObject(handle, msec) == WAIT_OBJECT_0) {
		return 0;
	}
	return -1;
}

void unlock_mutex(mutex_t handle)
{
	if (handle) {
		ReleaseMutex(handle);
	}
}

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
    free(pth);
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
    pthread_exit(NULL);
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

void destory_mutex(mutex_t handle)
{
    pthread_mutex_destroy(&handle);
}

int lock_mutex(mutex_t handle)
{
	return (pthread_mutex_lock(&handle) == 0) ? 0 : -1;
}

int try_lock_mutex(mutex_t handle, unsigned int msec)
{
	timelong_t start;
	int rt;

	if ((rt = pthread_mutex_trylock(&handle)) == EBUSY) {
		get_cur_timelong(&start);
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
