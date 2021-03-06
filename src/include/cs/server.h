/**
 * @file server.h
 * @brief This file provide some helper functions for server.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-26
 * @modified  Wed 2016-01-06 22:37:45 (+0800)
 */

#ifndef  SERVER_HELPER_H
#define  SERVER_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif


struct jxserver
{
    jxsock_t hsock;
    struct sockaddr_in sa_in;
    char* prompt;
};

/**
 * @brief  jxserver_init
 *
 * @param serv
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set, upd socket will be created if SOCK_DGRAM is set.
 *
 * @note This function must be called after init_sock_environment being called. 
 */
JXIOT_API void jxserver_init(struct jxserver* serv, int tcpudp, u_short port, u_long addr);

/**
 * @brief  jxserver_udp This function process communication for udp socket.
 *
 * @param serv
 */
JXIOT_API void jxserver_udp(struct jxserver* serv);

JXIOT_API ssize_t jxserver_recv(jxsock_t handle, void* inbuf, size_t inbytes);

JXIOT_API int jxserver_send(jxsock_t handle, void* sendbuf);

JXIOT_API void jxserver_msgpool_dispatch_init(struct jxserver* serv, struct list_head* plist_head);


#ifdef __cplusplus
}
#endif

#endif  // SERVER_HELPER_H
