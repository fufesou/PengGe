/**
 * @file main_listbuf.c
 * @brief  This demo routine use 'struct array_buf' as the buffer struct.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-14
 */

#include  <stdio.h>
#include  <windows.h>
#include  <process.h>
#include  <semaphore.h>
#include    "list.h"
#include    "macros.h"
#include    "buflist.h"

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

static LIST_HEAD(filled_listbuf);
static LIST_HEAD(empty_listbuf);

static void create_semophare(long count_init, long count_max);
static void create_threads(int num_thread);
static unsigned int __stdcall processBufferData(void* pPM);
static void process_msg(struct buf_node* bufnode, long threadid);


int main(void)
{
    int i;
    int numbuf = 32;
    struct buf_node* bufnode = NULL;

    init_buflist(&empty_listbuf, numbuf, 255);

    InitializeCriticalSection(&cs_code);

    create_semophare(0, numbuf);
    create_threads(NUM_THREAD);

    while (fgets(buf, sizeof(buf), stdin) != NULL)
    {
        i = 0;
        while (i < numbuf)
        {
            EnterCriticalSection(&cs_code);

            if ((bufnode = pullbuf(&empty_listbuf)) == NULL)
            {
                LeaveCriticalSection(&cs_code);
                Sleep(10);
                EnterCriticalSection(&cs_code);
                continue;
            }
            sprintf(bufnode->buf, "%d", i);
            pushbuf(bufnode, &filled_listbuf);

            LeaveCriticalSection(&cs_code);
            ReleaseSemaphore(handle_semfilled, 1, NULL);
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

void process_msg(struct buf_node* bufnode, long threadid)
{
    printf("thread id: %ld, handle message: %s.\n", threadid, bufnode->buf);
}

unsigned int __stdcall processBufferData(void* pPM)
{
    struct buf_node* bufnode = NULL;

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
        bufnode = pullbuf(&filled_listbuf);
#ifdef _DEBUG
        ++pullcount;
#endif
        if (bufnode == NULL) continue;
        LeaveCriticalSection(&cs_code);

        process_msg(bufnode, GetCurrentThreadId());
        Sleep(100);

        EnterCriticalSection(&cs_code);
#ifdef _DEBUG
        ++pushcount;
#endif
        pushbuf(bufnode, &empty_listbuf);
        LeaveCriticalSection(&cs_code);
    }

    return 0;
}
