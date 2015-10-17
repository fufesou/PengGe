/**
 * @file main_arraybuf.c
 * @brief  This demo routine use 'struct array_buf' as the buffer struct.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-17
 */

#include  <assert.h>
#include  <stdio.h>
#include  <windows.h>
#include  <process.h>
#include  <semaphore.h>
#include    "list.h"
#include    "macros.h"
#include    "bufarray.h"

#define NUM_THREAD 4

HANDLE handle_thread[NUM_THREAD];
HANDLE handle_semfilled;
char buf[255];
CRITICAL_SECTION cs_code;

#ifdef _DEBUG
int pullcount = 0;
int pushcount = 0;
int countsem_wait = 0;
#endif

static struct array_buf filled_arraybuf;
static struct array_buf empty_arraybuf;

static void create_semophare(long count_init, long count_max);
static void create_threads(int num_thread);
static unsigned int __stdcall processBufferData(void* pPM);
static void process_msg(char* bufitem, long threadid);


int main(void)
{
    int i;
    int numbuf = 32;
    char* bufitem = NULL;

    init_buf(&filled_arraybuf, 30, 510, 0);
    init_buf(&empty_arraybuf, 30, 500, -1);

    InitializeCriticalSection(&cs_code);

    create_semophare(0, filled_arraybuf.num_item - 1);
    create_threads(NUM_THREAD);

    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        i = 0;
        while (i < numbuf)
        {
            EnterCriticalSection(&cs_code);
            if ((bufitem = empty_arraybuf.pull_item(&empty_arraybuf)) == NULL)
            {
                LeaveCriticalSection(&cs_code);
                Sleep(10);
                continue;
            }
            sprintf(bufitem, "%d", i);
            filled_arraybuf.push_item(&filled_arraybuf, bufitem);

            LeaveCriticalSection(&cs_code);
            ReleaseSemaphore(handle_semfilled, 1, NULL);
            // Sleep(10);
            ++i;
        }
    }

    WaitForMultipleObjects(NUM_THREAD, handle_thread, 1, INFINITE);
    for (i=0; i<NUM_THREAD; ++i)
    {
        CloseHandle(handle_thread[i]);
    }
    CloseHandle(handle_semfilled);
    DeleteCriticalSection(&cs_code);

    empty_arraybuf.clear_buf(&empty_arraybuf);
    filled_arraybuf.clear_buf(&filled_arraybuf);

    return 0;
}

void create_semophare(long count_init, long count_max)
{
    handle_semfilled = CreateSemaphore(NULL, count_init, count_max, NULL);
    if (handle_semfilled == NULL)
    {
        printf("create semaphore error! errno: %ld\n", GetLastError());
        exit(1);
    }
}

void create_threads(int num_thread)
{
    int i;
    for (i=0; i<num_thread; ++i)
    {
        handle_thread[i] = (HANDLE)_beginthreadex(NULL, 0, processBufferData, NULL, 0, NULL);
        while ((long)handle_thread[i] == 1L)
        {
            printf("create thread error, try again now\n");
            handle_thread[i] = (HANDLE)_beginthreadex(NULL, 0, processBufferData, NULL, 0, NULL);
        }
        if (handle_thread[i] == 0)
        {
            printf("create thread error, errno: %d.\nexit(1)\n", errno);
            exit(1);
        }
        Sleep(20);
    }
}

void process_msg(char* bufitem, long threadid)
{
    printf("thread id: %ld, handle message: %s.\n", threadid, bufitem);
}

unsigned int __stdcall processBufferData(void* pPM)
{
    char* bufitem = NULL;

    (void)pPM;

    printf("child thread %ld created.\n", GetCurrentThreadId());
    while (1) {
        WaitForSingleObject(handle_semfilled, INFINITE);

#ifdef _DEBUG
        EnterCriticalSection(&cs_code);
        ++countsem_wait;
        LeaveCriticalSection(&cs_code);
#endif

        EnterCriticalSection(&cs_code);
        bufitem = filled_arraybuf.pull_item(&filled_arraybuf);
#ifdef _DEBUG
        ++pullcount;
#endif
        assert(bufitem != NULL);
        if (bufitem == NULL) continue;
        LeaveCriticalSection(&cs_code);

        process_msg(bufitem, GetCurrentThreadId());
        Sleep(200);

        EnterCriticalSection(&cs_code);
#ifdef _DEBUG
        ++pushcount;
#endif
        empty_arraybuf.push_item(&empty_arraybuf, bufitem);
        LeaveCriticalSection(&cs_code);
    }

    return 0;
}
