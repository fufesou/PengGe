/**
 * @file client.h
 * @brief This file provide the basic helper funstion for client struct.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-30
 * @modified  Wed 2016-01-06 21:09:43 (+0800)
 */

#ifndef  CLIENT_UDP_H
#define  CLIENT_UDP_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * jxclient contains necessary members and functions for client operations.
 * This struct object must be initialized (by calling jxclient_init)
 *  before any operations applied to it.
 *
 * @sa jxclient_init
 */
struct jxclient
{
    /** hsock_sendrecv is the socket used for 'client-server-client' message. */
    jxsock_t hsock_sendrecv;

    /** hsock_recv is a blocked socket used for 'server-client' message. */
    jxsock_t hsock_recv;

    /** sockaddr_in is the server socket address. */
    struct sockaddr_in sa_in;

    /** msgheader is the string that print before message.
     *
     * For client, the msgheader is "client", the output of client look like "clinet: xxx".
     */
    char* prompt;

    /** sendbuf is the buffer to hold out message.
     *
     * @todo make sendbuf a pointer, and configure the length of sendbuf by user.
     */
    char sendbuf[MAX_MSG_LEN];
    uint32_t len_senddata;      /**< len_senddata gives the length of data to send. */
    uint32_t size_senbuf;       /**< size_sendbuf is used for mallocing sendbuf. */

    /** sendbuf is the buffer to hold the message from server.
     *
     * If size of message from server is greater than 1024 bytes,
     *  the client will cut off the tail contents and puts warning.
     *
     * @todo make recvbuf a pointer, and configure the length of recvbuf by user.
     */
    char recvbuf[MAX_MSG_LEN];
    uint32_t len_recvdata;      /**< len_recvdata gives the length of data received. */
    uint32_t size_recvbuf;      /**< size_recvbuf is used for mallocing recvbuf. */
 };


/**
 * @brief  jxclient_init
 *
 * @param cli
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set,
 *  upd socket will be created if SOCK_DGRAM is set.
 * @param pfunc_msgprocess The function to process message from server.
 * If pfunc_msgprocess is set NULL, the default process (print message information
 * on console) will be set.
 *
 * @note 
 * 1. This function must be called after init_sock_environment being called. 
 * 2. Socket is set to be non-blocking socket by default.
 *  Call jxsock_block explicitly if you need the socket to be blocking.
 *
 * @sa jxsock_block jxsock_open
 */
JXIOT_API void jxclient_init(struct jxclient* cli, int tcpudp);

/**
 * @brief  jxclient_clear  There may be some clear works after communication,
 *  such as free the memory.
 *
 * @param cli
 */
JXIOT_API void jxclient_clear(void* cli);

JXIOT_API void jxclient_connect(jxsock_t hsock, const char* prompt, const struct sockaddr* servaddr, jxsocklen_t addrlen);

/**
 * @brief  jxclient_print jxclient_print will test and print client socket information.
 * If test fail, process will clear the socket environmtn and exit.
 *
 * @param hsock
 * @param prompt
 *
 * @return 
 * 1. JX_INVALID_ARGS(-1), if socket is not a valid socket.
 * 2. JX_NO_ERR(0), if socket is connected.
 * 3. JX_WARNING(1), if socket is not connected.
 *
 * @sa jxsock_print
 */
JXIOT_API int jxclient_print(jxsock_t hsock, const char* prompt);

/**
 * @brief  jxclient_thread_recv jxclient_thread_recv will create a thread function that listen message from server.
 *
 * @param cli
 * @param servaddr
 * @param addrlen
 *
 * @return the thread create return value.   
 *
 * @note this function works with udp socket.
 */
JXIOT_API int jxclient_thread_recv(struct jxclient* cli, const struct sockaddr* servaddr, jxsocklen_t addrlen);

/**
 * @brief  jxclient_udp This function process udp communication with server. This function call jxclient_udp_once in a loop.
 *
 * @param cli
 * @param fp        is the FILE pointer where input data comes from.
 * @param servaddr
 * @param addrlen
 *
 * @sa jxclient_udp_once
 *
 * @note            the first byte of message buffer is the mflag.
 */
JXIOT_API void jxclient_udp(struct jxclient* cli, FILE* fp, const struct sockaddr* servaddr, jxsocklen_t addrlen);

/**
 * @brief  jxclient_udp_once This function process udp send&recv one time.
 *
 * @param cli       cli has already been filled with data(end with '\0') to send.
 * @param servaddr
 * @param addrlen
 * @param mflag		the message flag. @sa jxiot.h
 *
 * @note            the first byte of message buffer is the mflag.
 */
JXIOT_API void jxclient_udp_once(struct jxclient* cli, const struct sockaddr* servaddr, jxsocklen_t addrlen, mflag_t mflag);

/**
 * @brief  jxclient_sendrecv
 *
 * @param cli
 * @param servaddr
 * @param addrlen
 * @param mflag		the message flag. @sa jxiot.h
 *
 * @return   
 * 1. number of bytes received.
 * 2. -1, if timeout.
 * 3. -2, if sendto error occurs.
 * 4. -3, if recvfrom error occurs.
 */
JXIOT_API ssize_t jxclient_sendrecv(struct jxclient* cli, const struct sockaddr* servaddr, jxsocklen_t addrlen, mflag_t mflag);

JXIOT_API void jxclient_msgpool_dispatch_init(struct list_head* plist_head);

#ifdef __cplusplus
}
#endif


#endif  // CLIENT_UDP_H

