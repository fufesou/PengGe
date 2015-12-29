/**
 * @file lightthread.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Sun 2015-12-06 18:20:24 (+0800)
 */

#ifdef WIN32
#include  <Windows.h>
#include  <process.h>
#else
#include  <errno.h>
#include  <pthread.h>
#include  <unistd.h>
#include  <sys/time.h>
#endif

#ifndef _MSC_VER  /* *nix */
#include  <semaphore.h>
#endif

#include  <time.h>
#include  <stdio.h>
#include  <string.h>

#include    "common/jxiot.h"
#include    "common/lightthread.h"
#include    "common/timespan.h"

#ifdef __cplusplus
extern "C"
{
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
int csthread_create(pthread_proc_t proc, void* pargs, csthread_t* handle)
{
    *handle = (void*)_beginthreadex(NULL, 0, proc, pargs, 0, NULL);
    while ((long)(*handle) == 1L) {
        printf("create thread error, try again now\n");
        *handle = (void*)_beginthreadex(NULL, 0, proc, pargs, 0, NULL);
    }
    if ((*handle) == 0) {
        printf("create thread error, errno: %d.\nexit(1)\n", errno);
        return -1;
    }
    return 0;
}

void csthread_exit(void)
{
    _endthread();
}

void csthread_wait_terminate(csthread_t handle)
{
    DWORD waitStat = WaitForSingleObject(handle, INFINITE);
    if (waitStat != WAIT_OBJECT_0) {
        fprintf(stderr, "wait thread terminate failed, return state: %ld.", waitStat);
        if (waitStat == WAIT_FAILED) {
            fprintf(stderr, ", error code: %ld\n.", GetLastError());
        } else {
            fprintf(stderr, "\n.");
        }
        return;
    }
    CloseHandle(handle);
}

void csthreadN_wait_terminate(csthread_t* handle, int count)
{
    DWORD waitStat = WaitForMultipleObjects(count, handle, 1, INFINITE);
    if (waitStat != WAIT_OBJECT_0) {
        fprintf(stderr, "wait thread terminate failed, return state: %ld.", waitStat);
        if (waitStat == WAIT_FAILED) {
            fprintf(stderr, ", error code: %ld\n.", GetLastError());
        } else {
            fprintf(stderr, "\n.");
        }
        return;
    }

    {
        int i = 0;
        for (i=0; i<count; ++i) {
            CloseHandle(handle[i]);
        }
    }
}

unsigned int csthread_getpid(void)
{
    return GetCurrentThreadId();
}

void cssleep(unsigned int msec)
{
    Sleep(msec);
}


/**************************************************
 **             WIN32: the mutex block           **
 **************************************************/
csmutex_t csmutex_create(void)
{
    csmutex_t handle;
    if ((handle = CreateMutex(NULL, FALSE, NULL)) == NULL) {
        fprintf(stderr, "create thread fail, error code: %ld\n", GetLastError());
    }
    return handle;
}

void csmutex_destroy(csmutex_t* handle)
{
    if (*handle) {
        CloseHandle(*handle);
    }
}

int csmutex_lock(csmutex_t* handle)
{
    if ((*handle) && WaitForSingleObject(*handle, INFINITE) == WAIT_OBJECT_0) {
        return 0;
    }
    return -1;
}

int csmutex_try_lock(csmutex_t* handle, unsigned int msec)
{
    if ((*handle) && WaitForSingleObject(*handle, msec) == WAIT_OBJECT_0) {
        return 0;
    }
    return -1;
}

void csmutex_unlock(csmutex_t* handle)
{
    if (*handle) {
        ReleaseMutex(*handle);
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
            fprintf(stderr, ".\n");
        }
        return -1;
    }
    return 0;
}

int cssem_post(cssem_t* handle)
{
    if (ReleaseSemaphore(*handle, 1, NULL) == 0) {
        fprintf(stderr, "post semaphore falied, error code: %ld.\n", GetLastError());
        return -1;
    }
    return 0;
}

int cssem_destroy(cssem_t* handle)
{
    BOOL stat = CloseHandle(*handle);
    if (stat == 0) {
        fprintf(stderr, "close semaphore handle failed, error code: %ld.\n", GetLastError());
        return 1;
    }
    return 0;
}


#endif


/**
 * @brief The following block describe the unix thread functions.
 *
 */
#ifndef WIN32


/**************************************************
 **             UNIX: the thread block           **
 **************************************************/
int csthread_create(pthread_proc_t proc, void* pargs, csthread_t* handle)
{
    return pthread_create(handle, NULL, proc, pargs);
}

void csthread_exit(void)
{
    pthread_exit(NULL);
}

void csthread_wait_terminate(csthread_t handle)
{
    void* thread_result;
    if (pthread_join(handle, &thread_result) != 0) {
        perror("pthread_join falied\n");
        // exit(1);
    }
}

void csthreadN_wait_terminate(csthread_t* handle, int count)
{
    int i = 0;
    for (; i<count; ++i) {
        csthread_wait_terminate(handle[i]);
    }
}

unsigned int csthread_getpid(void)
{
    return pthread_self();
}

void cssleep(unsigned int msec)
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
csmutex_t csmutex_create(void)
{
    csmutex_t handle;
    pthread_mutex_init(&handle, NULL);
    return handle;
}

void csmutex_destroy(csmutex_t* handle)
{
    pthread_mutex_destroy(handle);
}

int csmutex_lock(csmutex_t* handle)
{
    return (pthread_mutex_lock(handle) == 0) ? 0 : -1;
}

int csmutex_try_lock(csmutex_t* handle, unsigned int msec)
{
    cstimelong_t start;
    int rt;

    if ((rt = pthread_mutex_trylock(handle)) == EBUSY) {
        cstimelong_cur(&start);
        while (rt == EBUSY) {
            if (cstimelong_span_millisec(&start) > msec) {
                rt = -1;
            } else {
                usleep(2000);
                rt = pthread_mutex_trylock(handle);
            }
        }
    }

    return rt;
}

void csmutex_unlock(csmutex_t* handle)
{
    pthread_mutex_unlock(handle);
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
