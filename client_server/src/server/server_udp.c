/**
 * @file csserver.c
 * @brief This file must be reimplement when config parser was done.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 * @modified  Wed 2015-11-04 19:14:48 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#include  <process.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <pthread.h>
#include  <unistd.h>
#endif

#include  <assert.h>
#include  <stdlib.h>
#include  <stdio.h>
#include  <stdint.h>
#include  <semaphore.h>
#include	"macros.h"
#include    "bufarray.h"
#include    "sock_types.h"
#include    "lightthread.h"
#include    "sendrecv_pool.h"
#include    "sock_wrap.h"
#include    "msgwrap.h"
#include    "server.h"
#include    "msgdispatch.h"


#ifdef __cplusplus
extern "C" {
#endif

static struct cssendrecv_pool s_sendpool, s_recvpool;

static void s_init_sendpool(cssock_t handle);
static void s_init_recvpool(cssock_t handle);

#ifdef WIN32
static unsigned int __stdcall s_process_recv(void* unused);
static unsigned int __stdcall s_process_send(void* unused);
#else
static void* s_process_recv(void* unused);
static void* s_process_send(void* unused);
#endif

#ifdef __cplusplus
}
#endif

void csserver_udp(struct csserver* serv)
{
    char* buf = NULL;
	int numbytes = 0;

    s_init_sendpool(serv->hsock);
    s_init_recvpool(serv->hsock);

    printf("%s: I\'m ready to receive a datagram...\n", serv->msgheader);
    while (1) {

		/*
		 * @brief block untile one buffer avaliable.
		 */
		while ((buf = cspool_pullitem(&s_recvpool, &s_recvpool.empty_buf)) == NULL)
					;

        numbytes = csserver_recv(serv->hsock, buf, s_recvpool.len_item);
        if (numbytes > 0) {

			/*
			 * @brief Push to pool will succeed in normal case. There is no need to test the return value.
			 */
			cspool_pushitem(&s_recvpool, &s_recvpool.filled_buf, buf);
        }
        else if (numbytes <= 0) {
            printf("%s: connection closed with error code: %d\n", serv->msgheader, cssock_get_last_error());
            break;
        }
        else {
            printf("%s: recvfrom() failed with error code: %d\n", serv->msgheader, cssock_get_last_error());
            break;
        }
    }

    printf("%s: finish receiving. closing the listening socket...\n", serv->msgheader);

    cspool_clear(&s_sendpool);
    cspool_clear(&s_recvpool);
}

void s_init_sendpool(cssock_t handle)
{
    cspool_init(&s_sendpool, MAX_MSG_LEN + sizeof(struct csmsg_header), SERVER_POOL_NUM_ITEM, NUM_THREAD, handle, s_process_send);
}

void s_init_recvpool(cssock_t handle)
{
    cspool_init(&s_recvpool, MAX_MSG_LEN + sizeof(struct csmsg_header), SERVER_POOL_NUM_ITEM, NUM_THREAD, handle, s_process_recv);
}

#ifdef WIN32
unsigned int __stdcall s_process_recv(void* unused)
#else
void* s_process_recv(void* unused)
#endif
{
	int outmsglen;
	char* msgbuf = NULL;
	char* outmsg = NULL;

    (void)unused;
    printf("child thread %d created.\n", csthread_getpid());

    while ((msgbuf = cspool_pullitem(&s_recvpool, &s_recvpool.filled_buf)) == NULL)
	  ;
    while ((outmsg = cspool_pullitem(&s_sendpool, &s_sendpool.empty_buf)) == NULL)
	  ;

    printf("recv- thread id: %d, process message: %s.\n", csthread_getpid(), msgbuf + sizeof(struct csmsg_header));

    outmsglen = s_sendpool.len_item;
    csserver_process_msg(msgbuf, outmsg, &outmsglen);
	
    while ((cspool_pushitem(&s_recvpool, &s_recvpool.empty_buf, msgbuf)) == NULL)
	  ;
    while ((cspool_pushitem(&s_sendpool, &s_sendpool.filled_buf, outmsg)) != NULL)
	  ;

    return 0;
}

#ifdef WIN32
unsigned int __stdcall s_process_send(void* unused)
#else
void* s_process_send(void* unused)
#endif
{
    char* outmsg = NULL;

    (void)unused;
    printf("child thread %d created.\n", csthread_getpid());

    while ((outmsg = cspool_pullitem(&s_sendpool, &s_sendpool.filled_buf)) == NULL)
	  ;

    csserver_send(s_sendpool.socket, outmsg);
	cssleep(200);

    cspool_pushitem(&s_sendpool, &s_sendpool.empty_buf, outmsg);

    return 0;
}
