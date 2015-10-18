/**
 * @file server_servroutine.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 */

#include  <assert.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <winsock2.h>
#include  <windows.h>
#include  <process.h>
#include  <semaphore.h>
#include    "bufarray.h"
#include    "udp_types.h"
#include    "server_sendrecv.h"
#include	"server_udp.h"
#include    "server_servroutine.h"
#include    "msgdispatch.h"

#ifdef __cplusplus
extern "C" {
#endif

static struct sendrecv_pool {
    HANDLE hsem_filled;
    HANDLE* hthreads;
    int num_thread;
    int len_item;
    int num_item;
    struct array_buf filled_buf;
    struct array_buf empty_buf;
}send_buf, recv_buf;

/**
 * @brief s_cscode is used to ensure some variable operation to be atom op. 
 */
static CRITICAL_SECTION s_cscode;

static void s_initbuf(struct sendrecv_pool* buf);
static void s_clearbuf(struct sendrecv_pool* buf);
static void s_create_semaphore(long count_init, long count_max);
static void s_create_threads(int num_thread);
static unsigned int __stdcall s_process_buffer(void* unused);

#ifdef __cplusplus
}
#endif

void process_communication(struct server_udp* serv_udp)
{
    struct hdr hdrdata;
    int cli_addrlen;
    ssize_t byte_received;
    SOCKADDR_IN cli_addr;

    s_initbuf(&send_buf);
    s_initbuf(&recv_buf);

    printf("%s: I\'m ready to receive a datagram...\n", serv_udp->msgheader);
    while (1) {
        cli_addrlen = sizeof(cli_addr);
        byte_received = server_recv(
                    serv_udp->socket,
                    serv_udp->msgbuf,
                    sizeof(serv_udp->msgbuf),
                    &hdrdata,
                    (struct sockaddr*)&cli_addr,
                    &cli_addrlen);
        if (byte_received > 0) {
            printf("%s: total bytes received: %d\n", serv_udp->msgheader, byte_received);
            printf("%s: the data is \"%s\"\n", serv_udp->msgheader, serv_udp->msgbuf);

#ifdef _DEBUG
            getpeername(serv_udp->socket, (SOCKADDR*)&cli_addr, &cli_addrlen);
            printf("%s: sending IP used: %s\n", serv_udp->msgheader, inet_ntoa(cli_addr.sin_addr));
            printf("%s: sending port used: %d\n", serv_udp->msgheader, htons(cli_addr.sin_port));
#endif

            // server_send(serv_udp->socket, g_loginmsg_FAIL, strlen(g_loginmsg_FAIL) + 1, hdrdata, (SOCKADDR*)to_sockaddr, sockaddr_len);
        }
        else if (byte_received <= 0) {
            printf("%s: connection closed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
        else {
            printf("%s: recvfrom() failed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
    }

    printf("%s: finish receiving. closing the listening socket...\n", serv_udp->msgheader);

    s_clearbuf(&send_buf);
    s_clearbuf(&recv_buf);
}

void s_initbuf(struct sendrecv_pool* buf)
{
    const int init_fillednum = 0;
    int init_emptynum = 0;

    init_buf(&s_filledbuf, s_bufnum, s_buflen, init_fillednum);
    init_emptynum = s_filledbuf.num_item - 1 - s_filledbuf.get_num_contained_item(&s_filledbuf);
    init_buf(&s_emptybuf, s_bufnum, s_buflen, init_emptynum);

    InitializeCriticalSection(&s_cscode);

    s_create_threads(s_threadnum);
    s_create_semaphore(init_fillednum, s_filledbuf.num_item - 1);
}

void s_clearbuf(struct sendrecv_pool* buf)
{
    int i = 0;

    WaitForMultipleObjects(s_threadnum, s_hthreads, 1, INFINITE);
    for (i=0; i<s_threadnum; ++i) {
        CloseHandle(s_hthreads[i]);
    }

    CloseHandle(s_hsemfilled);

    DeleteCriticalSection(&s_cscode);

    s_filledbuf.clear_buf(&s_filledbuf);
    s_emptybuf.clear_buf(&s_emptybuf);
}

void s_create_semaphore(long count_init, long count_max)
{
    s_hsemfilled = CreateSemaphore(NULL, count_init, count_max, NULL);
    if (s_hsemfilled == NULL) {
        printf("create semaphore error! errno: %ld\n", GetLastError());
        exit(1);
    }
}

void s_create_threads(int num_thread)
{
    int i;
    s_hthreads = (HANDLE*)malloc(sizeof(HANDLE) * num_thread);
    for (i=0; i<num_thread; ++i) {
        s_hthreads[i] = (HANDLE)_beginthreadex(NULL, 0, s_process_buffer, NULL, 0, NULL);
        while ((long)s_hthreads[i] == 1L) {
            printf("create thread error, try again now\n");
            s_hthreads[i] = (HANDLE)_beginthreadex(NULL, 0, s_process_buffer, NULL, 0, NULL);
        }
        if (s_hthreads[i] == 0) {
            printf("create thread error, errno: %d.\nexit(1)\n", errno);
            exit(1);
        }
        Sleep(20);
    }
}

unsigned int __stdcall s_process_buffer(void* unused)
{
    char* bufitem = NULL;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(s_hsemfilled, INFINITE);

        EnterCriticalSection(&s_cscode);
        bufitem = s_filledbuf.pull_item(&s_filledbuf);
        assert(bufitem != NULL);
        if (bufitem == NULL) continue;
        LeaveCriticalSection(&s_cscode);

#ifdef _DEBUG
        printf("thread id: %ld, handle message: %s.\n", GetCurrentThreadId(), bufitem);
#endif
        Sleep(200);

        EnterCriticalSection(&s_cscode);
        s_emptybuf.push_item(&s_emptybuf, bufitem);
        LeaveCriticalSection(&s_cscode);
    }

    return 0;
}
