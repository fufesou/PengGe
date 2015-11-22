/**
 * @file test_thread.c
 * @brief  This test project is used to test mutex, semaphore, and thread...
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-11-22
 * @modified  Sun 2015-11-22 11:11:52 (+0800)
 */

#ifdef WIN32
#include  <Windows.h>
#include  <process.h>
#else
#include  <errno.h>
#include  <semaphore.h>
#include  <pthread.h>
#include  <unistd.h>
#include  <sys/time.h>
#endif

#include  <time.h>
#include  <stdio.h>
#include  <malloc.h>
#include  <string.h>
#include  <semaphore.h>

#include  <stdio.h>
#include  <stdlib.h>

#include    "../../src/common/lightthread.h"


#ifdef WIN32
	static unsigned int __stdcall s_test_thread_func(void* unused);
#else
	static void* s_test_thread_func(void* unused);
#endif

static int s_count;
static csmutex_t s_mutex;
static csthread_t s_hthread[4];


int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    s_mutex = csmutex_create();

	csthread_create(s_test_thread_func, NULL, &s_hthread[0]);
	csthread_create(s_test_thread_func, NULL, &s_hthread[1]);
	csthread_create(s_test_thread_func, NULL, &s_hthread[2]);

    while (1)
        ;

	return 0;
}

#ifdef WIN32
static unsigned int __stdcall s_test_thread_func(void* unused) {
#else
static void* s_test_thread_func(void* unused) {
#endif
	int i = 0;
	(void)unused;
    while (i++ < 10) {
        if (csmutex_lock(&s_mutex) != 0) {
            printf("mutex lock error.\n");
        }
        printf("thread - %d, count - %d.\n", csthread_getpid(), s_count+=3);
        usleep(50 * 1000);
        printf("thread - %d, count - %d.\n", csthread_getpid(), --s_count);
        usleep(50 * 1000);
        printf("thread - %d, count - %d.\n", csthread_getpid(), --s_count);
        usleep(50 * 1000);
        csmutex_unlock(&s_mutex);
    }

	return 0;
}
