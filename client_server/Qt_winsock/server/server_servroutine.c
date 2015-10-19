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
#include	"macros.h"
#include    "bufarray.h"
#include    "udp_types.h"
#include    "server_sendrecv.h"
#include	"server_udp.h"
#include    "server_servroutine.h"
#include    "msgdispatch.h"
#include    "sendrecv_pool.h"
#include	"msgunit.h"


#ifdef __cplusplus
extern "C" {
#endif

static struct sendrecv_pool s_sendpool, s_recvpool;

/**
 * @brief s_cscode is used to ensure some variable operation to be atom op. 
 */
static CRITICAL_SECTION s_cscode;

/**
 * @brief  s_init_sendpool should parse the config first, and then set data to s_sendpool.
 */
static void s_init_sendpool(void);
/**
 * @brief  s_init_recvpool should parse the config first, and then set data to s_recvpool.
 *
 */
static void s_init_recvpool(void);
static void s_push2pool(const char* data, const struct unit_header* unithdr, struct sendrecv_pool* pool);
static unsigned int __stdcall s_process_recv(void* unused);
static unsigned int __stdcall s_process_send(void* unused);

#ifdef __cplusplus
}
#endif

void process_communication(struct server_udp* serv_udp)
{
    struct unit_header unithdr;
    char buf[MAX_MSG_LEN];

    /**
     * @brief  critical section must be initialized first of all. 
     *
     */
    InitializeCriticalSection(&s_cscode);

    s_init_sendpool();
    s_init_recvpool();

    printf("%s: I\'m ready to receive a datagram...\n", serv_udp->msgheader);
    while (1) {
        unithdr.addrlen = sizeof(unithdr.addr);
        unithdr.numbytes = server_recv(
                    serv_udp->socket,
                    buf,
                    sizeof(buf),
                    &unithdr.header,
                    (struct sockaddr*)&unithdr.addr,
                    &unithdr.addrlen);
        if (unithdr.numbytes > 0) {

#ifdef _DEBUG
            printf("%s: total bytes received: %d\n", serv_udp->msgheader, unithdr.numbytes);
            printf("%s: the data is \"%s\"\n", serv_udp->msgheader, buf);

            getpeername(serv_udp->socket, (SOCKADDR*)&unithdr.addr, &unithdr.addrlen);
            printf("%s: sending IP used: %s\n", serv_udp->msgheader, inet_ntoa(unithdr.addr.sin_addr));
            printf("%s: sending port used: %d\n", serv_udp->msgheader, htons(unithdr.addr.sin_port));
#endif
            s_push2pool(buf, &unithdr, &s_recvpool);
        }
        else if (unithdr.numbytes <= 0) {
            printf("%s: connection closed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
        else {
            printf("%s: recvfrom() failed with error code: %d\n", serv_udp->msgheader, WSAGetLastError());
            break;
        }
    }

    printf("%s: finish receiving. closing the listening socket...\n", serv_udp->msgheader);

    s_sendpool.clear_pool(&s_sendpool);
    s_recvpool.clear_pool(&s_recvpool);

    DeleteCriticalSection(&s_cscode);
}

void s_init_sendpool(void)
{
    init_sendrecv_pool(&s_sendpool, 512, 64, 4, s_process_send);
    s_sendpool.init_pool(&s_sendpool);
}

void s_init_recvpool(void)
{
    init_sendrecv_pool(&s_recvpool, 512, 64, 4, s_process_recv);
    s_recvpool.init_pool(&s_recvpool);
}

void s_push2pool(const char* data, const struct unit_header* unithdr, struct sendrecv_pool* pool)
{
    char* poolbuf = NULL;

    EnterCriticalSection(&s_cscode);
    if ((poolbuf = pool->empty_buf.pull_item(&pool->empty_buf)) == NULL) {
        LeaveCriticalSection(&s_cscode);
        return;
    }

    if (merge2unit(unithdr, data, poolbuf, pool->len_item) != 0)
    {
        printf("copy data to pool error, omit current data.\n");
        pool->empty_buf.push_item(&pool->empty_buf, poolbuf);
        LeaveCriticalSection(&s_cscode);
        return;
    }

    pool->filled_buf.push_item(&pool->filled_buf, poolbuf);
    LeaveCriticalSection(&s_cscode);

    ReleaseSemaphore(pool->hsem_filled, 1, NULL);
}

unsigned int __stdcall s_process_recv(void* unused)
{
    char* bufitem = NULL;
    char* msgdata = NULL;
    char outmsg[MAX_MSG_LEN];
    const struct unit_header* unithdr = NULL;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(s_recvpool.hsem_filled, INFINITE);

        EnterCriticalSection(&s_cscode);
        bufitem = s_recvpool.filled_buf.pull_item(&s_recvpool.filled_buf);
        LeaveCriticalSection(&s_cscode);
        if (bufitem == NULL) {
            continue;
        }

#ifdef _DEBUG
        printf("recv- thread id: %ld, handle message: %s.\n", GetCurrentThreadId(), bufitem);
#endif
        extract_msg(bufitem, &unithdr, (const char**)&msgdata);

        EnterCriticalSection(&s_cscode);
        s_recvpool.empty_buf.push_item(&s_recvpool.empty_buf, bufitem);
        LeaveCriticalSection(&s_cscode);

        process_msg(msgdata, outmsg, sizeof(outmsg));
        Sleep(200);


#ifdef WIN32
        strcat_s(outmsg, sizeof(outmsg), "server: ");
#else
        strcat(outmsg, "server: ");
#endif

        s_push2pool(outmsg, unithdr, &s_sendpool);
    }

    return 0;
}

unsigned int __stdcall s_process_send(void* unused)
{
    char* bufitem = NULL;

    (void)unused;
    printf("child thread %ld created.\n", GetCurrentThreadId());

    while (1) {
        WaitForSingleObject(s_sendpool.hsem_filled, INFINITE);

        EnterCriticalSection(&s_cscode);
        bufitem = s_sendpool.filled_buf.pull_item(&s_sendpool.filled_buf);
        LeaveCriticalSection(&s_cscode);
        if (bufitem == NULL) {
            continue;
        }

#ifdef _DEBUG
        printf("send- thread id: %ld, handle message: %s.\n", GetCurrentThreadId(), bufitem);
#endif
        // server_send(serv_udp->socket, g_loginmsg_FAIL, strlen(g_loginmsg_FAIL) + 1, hdrdata, (SOCKADDR*)to_sockaddr, sockaddr_len);
        Sleep(200);

        EnterCriticalSection(&s_cscode);
        s_sendpool.empty_buf.push_item(&s_sendpool.empty_buf, bufitem);
        LeaveCriticalSection(&s_cscode);
    }

    return 0;
}
