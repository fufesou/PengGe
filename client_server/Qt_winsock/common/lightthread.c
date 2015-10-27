/**
 * @file lightthread.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Tue 2015-10-27 19:54:03 (+0800)
 */

#include  <time.h>
#include  <stdio.h>
#include  <malloc.h>
#include  <string.h>

#ifdef WIN32
#include  <Windows.h>
#include  <process.h>
#include  <semaphore.h>
#else
#include  <errno.h>
#include  <semaphore.h>
#include  <pthread.h>
#include  <unistd.h>
#include  <sys/time.h>
#define THREAD_IDEL_TIMESLICE_MS 20
#endif

#include    "lightthread.h"
#include    "macros.h"
#include    "timespan.h"

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

/**************************************************
 **            WIN32: the thread block           **
 **************************************************/
int create_thread(void (*proc)(void*), void* pargs, csthread_t* handle)
{
    return (int)(*handle = (void*)_beginthread(proc, 0, pargs));
}

void exit_thread(void)
{
	_endthread();
}

void wait_thread_terminate(csthread_t handle)
{
    DWORD waitStat = WaitForSingleObject(handle, INFINITE);
    if (waitStat != WAIT_OBJECT_0) {
        fprintf(stderr, "wait thread terminate failed, return state: %ld", waitStat);
        if (waitStat == WAIT_FAILED) {
            fprintf(stderr, ", error code: %ld\n.", GetLastError());
        } else {
            fprintf(stderr, "\n.");
        }
    }
}

void wait_threadN_terminate(csthread_t* handle, int count)
{
    DWORD waitStat = WaitForMultipleObjects(count, handle, 1, INFINITE);
    if (waitStat != WAIT_OBJECT_0) {
        fprintf(stderr, "wait thread terminate failed, return state: %ld", waitStat);
        if (waitStat == WAIT_FAILED) {
            fprintf(stderr, ", error code: %ld\n.", GetLastError());
        } else {
            fprintf(stderr, "\n.");
        }
    }
}

unsigned int get_pid(void)
{
	return GetCurrentThreadId();
}

void cs_sleep(unsigned int msec)
{
	Sleep(msec);
}


/**************************************************
 **             WIN32: the mutex block           **
 **************************************************/
csmutex_t create_mutex(void)
{
	csmutex_t handle;
	if ((handle = CreateMutex(NULL, FALSE, NULL)) == NULL) {
        fprintf(stderr, "create thread fail, error code: %ld\n", GetLastError());
	}
	return handle;
}

void destory_mutex(csmutex_t handle)
{
	if (handle) {
		CloseHandle(handle);
	}
}

int lock_mutex(csmutex_t handle)
{
	if (handle && WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0) {
		return 0;
	}
	return -1;
}

int try_lock_mutex(csmutex_t handle, unsigned int msec)
{
	if (handle && WaitForSingleObject(handle, msec) == WAIT_OBJECT_0) {
		return 0;
	}
	return -1;
}

void unlock_mutex(csmutex_t handle)
{
	if (handle) {
		ReleaseMutex(handle);
	}
}


/**************************************************
 **           WIN32: the semophare block         **
 **************************************************/
int cssem_create(int value_init, int value_max, cssem_t* handle)
{
	*handle = CreateSemaphore(NULL, value_init, value_max, NULL);
	if (*handle == NULL) {
		fprintf(stderr, "create semaphore failed, errno: %ld.\n", GetLastError());
		return -1;
	}
	return 0;
}

int cssem_wait(cssem_t* handle)
{
	DWORD waitStat = WaitForSingleObject(*handle, INFINITE);
	if (waitStat != WAIT_OBJECT_0) {
		fprintf(stderr, "wait semaphore failed, return state: %ld", waitStat);
		if (waitStat == WAIT_FAILED) {
            fprintf(stderr, ", error code: %ld\n.", GetLastError());
		} else {
            fprintf(stderr, "\n.");
		}
		return -1;
	}
	return 0;
}

int cssem_post(cssem_t* handle)
{
	if (ReleaseSemaphore(*handle, 1, NULL) == 0) {
        fprintf(stderr, ", post semaphoer falied, error code: %ld\n.", GetLastError());
		return -1;
	}
	return 0;
}

int cssem_destroy(cssem_t* handle)
{
	DWORD waitStat = WaitForSingleObject(handle, INFINITE);
	if (waitStat != WAIT_OBJECT_0) {
		fprintf(stderr, "wait thread terminate failed, return state: %ld", waitStat);
		if (waitStat == WAIT_FAILED) {
            fprintf(stderr, ", error code: %ld\n.", GetLastError());
		} else {
            fprintf(stderr, "\n.");
		}
		return 0;
	}
	return -1;
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


/**************************************************
 **             UNIX: the thread block           **
 **************************************************/
int create_thread(void (*proc)(void*), void* pargs, csthread_t* handle)
{
	threadwraper_linux_t* pwraper = (threadwraper_linux_t*)malloc(sizeof(threadwraper_linux_t));
	pwraper->proc = proc;
	pwraper->pargs = pargs;
	return pthread_create(handle, NULL, s_thread_process, pwraper);
}

void exit_thread(void)
{
    pthread_exit(NULL);
}

void wait_thread_terminate(csthread_t handle)
{
	void* thread_result;
	if (pthread_join(handle, &thread_result) != 0) {
		perror("pthread_join falied\n");
		// exit(1);
	}
}

void wait_threadN_terminate(csthread_t* handle, int count)
{
	int i = 0;
	for (; i<count; ++i) {
		wait_thread_terminate(handle[i]);
	}
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


/**************************************************
 **             UNIX: the mutex block            **
 **************************************************/
csmutex_t create_mutex(void)
{
	csmutex_t handle;
	pthread_mutex_init(&handle, NULL);
	return handle;
}

void destory_mutex(csmutex_t handle)
{
    pthread_mutex_destroy(&handle);
}

int lock_mutex(csmutex_t handle)
{
	return (pthread_mutex_lock(&handle) == 0) ? 0 : -1;
}

int try_lock_mutex(csmutex_t handle, unsigned int msec)
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

void unlock_mutex(csmutex_t handle)
{
	pthread_mutex_unlock(&handle);
}


/**************************************************
 **            UNIX: the semophare block         **
 **************************************************/
int cssem_create(int value_init, int value_max, cssem_t* handle)
{
	(void)value_max;
	if (-1 == sem_init(handle, 0, value_init)) {
		fprintf(stderr, "semaphore initialization falied, errno: %d\n", errno);
		// exit(1);
		return -1;
	}
	return 0;
}


#endif
