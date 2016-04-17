/**
 * @file main_arraybuf.c
 * @brief  This demo routine use 'struct array_buf' as the buffer struct.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-17
 */

#ifndef WIN32
#include  <pthread.h>
#include  <semaphore.h>
#endif

#include  <assert.h>
#include  <stdio.h>
#include    "macros.h"
#include    "bufarray.h"
#include    "lightthread.h"

#define NUM_THREAD 4

csmutex_t hmutex;
cssem_t hsem_filled;
csthread_t hthreads[NUM_THREAD];
char buf[255];

#ifdef _DEBUG
int pullcount = 0;
int pushcount = 0;
int countsem_wait = 0;
#endif

static struct array_buf filled_arraybuf;
static struct array_buf empty_arraybuf;

#ifdef WIN32
static unsigned int __stdcall processBufferData(void* param);
#else
static void* processBufferData(void* param);
#endif
static void process_msg(char* bufitem, long threadid);


int main(void)
{
    int i;
    int numbuf = 30;
    char* bufitem = NULL;

    init_buf(&filled_arraybuf, 30, 510, 0);
    init_buf(&empty_arraybuf, 30, 500, -1);

    hmutex = csmutex_create();
    cssem_create(0, filled_arraybuf.num_item - 1, &hsem_filled);

    for (i=0; i<NUM_THREAD; ++i) {
        csthread_create(processBufferData, NULL, hthreads + i);
    }

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        i = 0;
        while (i < numbuf) {
            csmutex_lock(hmutex);
            if ((bufitem = empty_arraybuf.pull_item(&empty_arraybuf)) == NULL) {
                csmutex_unlock(hmutex);
                cssleep(10);
                continue;
            }
            sprintf(bufitem, "%d", i);
            filled_arraybuf.push_item(&filled_arraybuf, bufitem);

            csmutex_unlock(hmutex);
            cssem_post(&hsem_filled);
            cssleep(10);
            ++i;
        }
    }

    csthreadN_wait_terminate(hthreads, NUM_THREAD);
    cssem_destroy(&hsem_filled);
    csmutex_destroy(hmutex);

    empty_arraybuf.clear_buf(&empty_arraybuf);
    filled_arraybuf.clear_buf(&filled_arraybuf);

    return 0;
}

void process_msg(char* bufitem, long threadid)
{
    printf("thread id: %ld, handle message: %s.\n", threadid, bufitem);
}

#ifdef WIN32
unsigned int __stdcall processBufferData(void* param)
#else
void* processBufferData(void* param)
#endif
{
    char* bufitem = NULL;

    (void)param;

    printf("child thread %d created.\n", csthread_getpid());
    while (1) {
        cssem_wait(&hsem_filled);

#ifdef _DEBUG
        csmutex_lock(hmutex);
        ++countsem_wait;
        csmutex_unlock(hmutex);
#endif

        csmutex_lock(hmutex);
        bufitem = filled_arraybuf.pull_item(&filled_arraybuf);
#ifdef _DEBUG
        ++pullcount;
#endif
        assert(bufitem != NULL);
        if (bufitem == NULL) continue;
        csmutex_unlock(hmutex);

        process_msg(bufitem, csthread_getpid());
        cssleep(200);

        csmutex_lock(hmutex);
#ifdef _DEBUG
        ++pushcount;
#endif
        empty_arraybuf.push_item(&empty_arraybuf, bufitem);
        csmutex_unlock(hmutex);
    }

    return 0;
}
