/**
 * @file test_thread.c
 * @brief  This test project is used to test mutex, semaphore, and thread...
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-11-22
 * @modified  Sun 2015-11-22 11:33:09 (+0800)
 */

#include  <errno.h>
#include  <semaphore.h>
#include  <pthread.h>
#include  <unistd.h>
#include  <sys/time.h>
#include  <stdio.h>
#include  <malloc.h>
#include  <string.h>
#include  <semaphore.h>
#include  <stdio.h>
#include  <stdlib.h>



static void* s_test_thread_func(void* unused);
static int s_count;
static pthread_mutex_t s_mutex;
static pthread_t s_hthread[4];


int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    pthread_mutex_init(&s_mutex, NULL);

    pthread_create(&s_hthread[0], NULL, s_test_thread_func, NULL);
    pthread_create(&s_hthread[1], NULL, s_test_thread_func, NULL);
    pthread_create(&s_hthread[2], NULL, s_test_thread_func, NULL);

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
        if (pthread_mutex_lock(&s_mutex) != 0) {
            printf("mutex lock error.\n");
        }
        printf("thread - %u, count - %d.\n", (unsigned int)pthread_self(), s_count+=3);
        usleep(50 * 1000);
        printf("thread - %u, count - %d.\n", (unsigned int)pthread_self(), --s_count);
        usleep(50 * 1000);
        printf("thread - %u, count - %d.\n", (unsigned int)pthread_self(), --s_count);
        usleep(50 * 1000);
        pthread_mutex_unlock(&s_mutex);
	}

	return 0;
}
