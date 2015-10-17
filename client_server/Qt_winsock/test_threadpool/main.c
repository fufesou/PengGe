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
CRITICAL_SECTION cs_param;

#ifdef _DEBUG
int pullcount = 0;
int pushcount = 0;
int countsem_wait = 0;
#endif

static LIST_HEAD(filled_listbuf);
static LIST_HEAD(empty_listbuf);

static void create_semophare(void);
static void create_threads(void);
static unsigned int __stdcall processBufferData(void* pPM);
static void process_msg(struct buf_node* bufnode, long threadid);


int main(void)
{
    int i;
    int numbuf = 32;
    struct buf_node* bufnode = NULL;

    init_buflist(&empty_listbuf, numbuf, 255);

    InitializeCriticalSection(&cs_code);
    InitializeCriticalSection(&cs_param);

    create_semophare();
    create_threads();

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

            // umcomment the following code.
            // if commented, this program will result in error:
            // the handler call times are less than 'numbuf'.
            // Sleep(10);
        }
    }

    WaitForMultipleObjects(NUM_THREAD, handle_thread, 1, INFINITE);
    for (i=0; i<NUM_THREAD; ++i)
    {
        CloseHandle(handle_thread[i]);
    }
    CloseHandle(handle_semfilled);
    DeleteCriticalSection(&cs_code);
    DeleteCriticalSection(&cs_param);

    return 0;
}

void create_semophare(void)
{
    handle_semfilled = CreateSemaphore(NULL, 0, 32*32, NULL);
    if (handle_semfilled == NULL)
    {
        printf("create semaphore error! errno: %ld\n", GetLastError());
        exit(1);
    }
}

void create_threads(void)
{
    int i;
    for (i=0; i<NUM_THREAD; ++i)
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
        LeaveCriticalSection(&cs_code);
        while (bufnode != NULL) {

#ifdef _DEBUG
        EnterCriticalSection(&cs_code);
        ++pullcount;
        LeaveCriticalSection(&cs_code);
#endif

            process_msg(bufnode, GetCurrentThreadId());
            Sleep(100);

            EnterCriticalSection(&cs_code);
#ifdef _DEBUG
            ++pushcount;
#endif
            pushbuf(bufnode, &empty_listbuf);
            LeaveCriticalSection(&cs_code);

            EnterCriticalSection(&cs_code);
            bufnode = pullbuf(&filled_listbuf);
            LeaveCriticalSection(&cs_code);
        }
    }

    return 0;
}
