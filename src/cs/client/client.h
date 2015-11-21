/**
 * @file client.h
 * @brief This file provide the basic helper funstion for client struct.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-30
 * @modified  Thu 2015-11-19 23:25:31 (+0800)
 */

#ifndef  CLIENT_UDP_H
#define  CLIENT_UDP_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * csclient contains necessary members and functions for client operations.
 * This struct object must be initialized (by calling csclient_init)
 *  before any operations applied to it.
 *
 * @sa csclient_init
 */
struct csclient
{
    cssock_t hsock;

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

    int (*pfunc_process_react)(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
 };


/**
 * @brief  csclient_init 
 *
 * @param cli
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set,
 *  upd socket will be created if SOCK_DGRAM is set.
 *
 * @note 
 * 1. This function must be called after init_sock_environment being called. 
 * 2. Socket is set to be non-blocking socket by default.
 *  Call cssock_block explicitly if you need the socket to be blocking.
 *
 * @sa cssock_block cssock_open
 */
void csclient_init(struct csclient* cli, int tcpudp);

/**
 * @brief  csclient_clear  There may be some clear works after communication,
 *  such as free the memory.
 *
 * @param cli
 */
void csclient_clear(void* cli);

void csclient_connect(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen);

/**
 * @brief  csclient_react_dispatch The income message from server 
 * will be handle by the coressponding 'react' method in this function.
 *
 * The actual process function will not process data until one byte after 'process id'. Thus
 * the message data passed into the process function is 'inmsg + sizeof(struct csmsg_header) + sizeof(uint32_t)'.
 *
 * @param inmsg The format of inmsg is: \n
 * ---------------------------------------------------------------------------------------------- \n
 * | struct csmsg_header | process id(uint32_t) | process data(char*) | ... |                     \n
 * ---------------------------------------------------------------------------------------------- \n
 * or \n
 * ------------------------------------------------------------------------------------------------------------------------------------ \n
 * | struct csmsg_header | process id(uint32_t) | user id(uint32_t) | process data(char*) | ... |                                       \n
 * ------------------------------------------------------------------------------------------------------------------------------------ \n
 * @param outmsg
 * @param outmsglen
 */
int csclient_react_dispatch(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);


/**
 * @brief  csclient_print csclient_print will test and print client socket information.
 * If test fail, process will clear the socket environmtn and exit.
 *
 * @param cli
 *
 * @return 
 * 1. -1, if socket is not a valid socket.
 * 2. 0, if socket is connected.
 * 3. 1, if socket is not connected.
 *
 * @sa cssock_print
 */
int csclient_print(const struct csclient* cli);

/**
 * @brief  csclient_udp This function process udp communication with server. This function call csclient_udp_once in a loop.
 *
 * @param cli
 * @param fp is the FILE pointer where input data comes from.
 * @param servaddr
 * @param addrlen
 *
 * @sa csclient_udp_once
 */
void csclient_udp(struct csclient* cli, FILE* fp, const struct sockaddr* servaddr, cssocklen_t addrlen);

/**
 * @brief  csclient_udp_once This function process udp send&recv one time.
 *
 * @param cli cli has already been filled with data(end with '\0') to send.
 * @param servaddr
 * @param addrlen
 */
void csclient_udp_once(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen);

/**
 * @brief  csclient_sendrecv 
 *
 * @param cli
 * @param servaddr
 * @param addrlen
 *
 * @return   
 * 1. number of bytes received.
 * 2. -1, if timeout.
 * 3. -2, if sendto error occurs.
 * 4. -3, if recvfrom error occurs.
 *
 * @sa cssendrecv_init cssendrecv_clear
 */
ssize_t csclient_sendrecv(struct csclient* cli, const struct sockaddr* servaddr, cssocklen_t addrlen);

void csclient_msgpool_dispatch_init(struct csclient* cli);

#ifdef __cplusplus
}
#endif


#endif  // CLIENT_UDP_H

