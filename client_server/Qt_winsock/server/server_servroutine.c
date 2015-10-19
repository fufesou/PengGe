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

typedef unsigned int __stdcall (*ptr_process_sendrecv)(void*);

/**
 * @brief sendrecv_pool contains basic members for send receive buffer and thread operations.
 * The members of struct sendrecv_pool should be set with configuration.
 *
 * @todo write a cofigure parser to set the members
 */
static struct sendrecv_pool {
    int num_thread;
    int len_item;
    int num_item;
    HANDLE* hthreads;
    HANDLE hsem_filled;
    struct array_buf filled_buf;
    struct array_buf empty_buf;

    ptr_process_sendrecv process_func;
}send_pool, recv_pool;

/**
 * @brief s_cscode is used to ensure some variable operation to be atom op. 
 */
static CRITICAL_SECTION s_cscode;

/**
 * @brief  s_configurate_sendpool should parse the config first, and then set data to send_pool.
 */
static void s_configurate_sendpool(void);
/**
 * @brief  s_configurate_recvpool should parse the config first, and then set data to recv_pool.
 *
 */
static void s_configurate_recvpool(void);
/**
 * @brief  s_initpool This function will initialize buffers, create semaphore and threads.
 *
 * @param pool The send or receive pool to be initialized.
 */
static void s_initpool(struct sendrecv_pool* pool);
/**
 * @brief  s_clearpool This function will do some clear works such as free memory.
 *
 * @param pool The send or receive pool to clear.
 */
static void s_clearpool(struct sendrecv_pool* pool);
static void s_create_semaphore(HANDLE* hsem, long count_init, long count_max);
static void s_create_threads(HANDLE** arr_hthreads, int num_thread, ptr_process_sendrecv process_func);
static unsigned int __stdcall s_process_recv(void* unused);
static unsigned int __stdcall s_process_send(void* unused);

#ifdef __cplusplus
}
#endif

void process_communication(struct server_udp* serv_udp)
{
    struct hdr hdrdata;
    int cli_addrlen;
    ssize_t byte_received;
    SOCKADDR_IN cli_addr;

    /**
     * @brief  critical section must be initialized first of all. 
     *
     */
    InitializeCriticalSection(&s_cscode);

    s_configurate_sendpool();
    s_configurate_recvpool();
    s_initpool(&send_pool);
    s_initpool(&recv_pool);

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

    s_clearpool(&send_pool);
    s_clearpool(&recv_pool);

    DeleteCriticalSection(&s_cscode);
}

void s_configurate_sendpool(void)
{
    send_pool.len_item = 512;
    send_pool.num_item = 64;
    send_pool.num_thread = 4;

    send_pool.process_func = s_process_send;
}

void s_configurate_recvpool(void)
{
    recv_pool.len_item = 512;
    recv_pool.num_item = 64;
    recv_pool.num_thread = 4;

    recv_pool.process_func = s_process_recv;
}

/**
 * @brief  s_initpool This function will initialize buffers, create semaphore and threads.
 *
 * @param pool The send or receive pool to be initialized.
 *
 * @note Correct sequence for initialize is required here.
 * 1. buffers
 * 2. semaphare
 * 3. threads
 */
void s_initpool(struct sendrecv_pool* pool)
{
    const int init_fillednum = 0;
    int num_items;
    int init_emptynum;

    init_buf(&pool->filled_buf, pool->num_item, pool->len_item, init_fillednum);

    num_items = pool->filled_buf.get_num_contained_item(&pool->filled_buf);
    init_emptynum = pool->filled_buf.num_item - 1 - num_items;
    init_buf(&pool->empty_buf, pool->num_item, pool->len_item, init_emptynum);

    s_create_semaphore(&pool->hsem_filled, 0, pool->filled_buf.num_item - 1);
    s_create_threads(&pool->hthreads, pool->num_thread, pool->process_func);
}

void s_clearpool(struct sendrecv_pool* pool)
{
    int i = 0;

    WaitForMultipleObjects(pool->num_thread, pool->hthreads, 1, INFINITE);
    for (i=0; i<pool->num_thread; ++i) {
        CloseHandle(pool->hthreads[i]);
    }

    CloseHandle(pool->hsem_filled);

    pool->filled_buf.clear_buf(&pool->filled_buf);
    pool->empty_buf.clear_buf(&pool->empty_buf);
}

void s_create_semaphore(HANDLE* hsem, long count_init, long count_max)
{
    *hsem = CreateSemaphore(NULL, count_init, count_max, NULL);
    if ((*hsem) == NULL) {
        printf("create semaphore error! errno: %ld\n", GetLastError());
        exit(1);
    }
}

void s_create_threads(HANDLE** arr_hthreads, int num_thread, ptr_process_sendrecv process_func)
{
    int i;
    *arr_hthreads = (HANDLE*)malloc(sizeof(HANDLE) * num_thread);
    for (i=0; i<num_thread; ++i) {
        (*arr_hthreads)[i] = (HANDLE)_beginthreadex(NULL, 0, process_func, NULL, 0, NULL);
        while ((long)(*arr_hthreads)[i] == 1L) {
            printf("create thread error, try again now\n");
            (*arr_hthreads)[i] = (HANDLE)_beginthreadex(NULL, 0, process_func, NULL, 0, NULL);
        }
        if ((*arr_hthreads)[i] == 0) {
            printf("create thread error, errno: %d.\nexit(1)\n", errno);
            exit(1);
        }
        Sleep(20);
    }
}

unsigned int __stdcall s_process_recv(void* unused)
{
    char* bufitem = NULL;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(recv_pool.hsem_filled, INFINITE);

        EnterCriticalSection(&s_cscode);
        bufitem = recv_pool.filled_buf.pull_item(&recv_pool.filled_buf);
        assert(bufitem != NULL);
        if (bufitem == NULL) continue;
        LeaveCriticalSection(&s_cscode);

#ifdef _DEBUG
        printf("recv- thread id: %ld, handle message: %s.\n", GetCurrentThreadId(), bufitem);
#endif
        Sleep(200);

        EnterCriticalSection(&s_cscode);
        recv_pool.empty_buf.push_item(&recv_pool.empty_buf, bufitem);
        LeaveCriticalSection(&s_cscode);
    }

    return 0;
}

unsigned int __stdcall s_process_send(void* unused)
{
    char* bufitem = NULL;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(send_pool.hsem_filled, INFINITE);

        EnterCriticalSection(&s_cscode);
        bufitem = send_pool.filled_buf.pull_item(&send_pool.filled_buf);
        assert(bufitem != NULL);
        if (bufitem == NULL) continue;
        LeaveCriticalSection(&s_cscode);

#ifdef _DEBUG
        printf("send- thread id: %ld, handle message: %s.\n", GetCurrentThreadId(), bufitem);
#endif
        Sleep(200);

        EnterCriticalSection(&s_cscode);
        send_pool.empty_buf.push_item(&send_pool.empty_buf, bufitem);
        LeaveCriticalSection(&s_cscode);
    }

    return 0;
}
