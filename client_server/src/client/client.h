/**
 * @file client.h
 * @brief This file provide the interfaces of udp client.
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 * @modified  Mon 2015-11-02 19:12:19 (+0800)
 */

#ifndef  CLIENT_UDP_H
#define  CLIENT_UDP_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief struct csclient contains necessary members and functions for client operations.
 * This struct object must be initialized (by calling csclient_init) before any operations applied to it.
 * @sa csclient_init
 */
struct csclient
{
    cssock_t hsock;

    /**
     * @brief sockaddr_in is the server socket address.
     */
    struct sockaddr_in sa_in;

    /**
     * @brief msgheader is the string that print before message.
     * For client, the msgheader is "client", the output of client look like "clinet: xxx".
     */
    char* msgheader;

    /**
     * @brief sendbuf is the buffer to hold out message.
     */
    char sendbuf[1024];

    /**
     * @brief sendbuf is the buffer to hold the message from server.
     * If size of message from server is greater than 1024 bytes, the client will cut off the tail contents and puts worning.
     */
    char recvbuf[1024];
 };


/**
 * @brief  csclient_init 
 *
 * @param cli
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set, upd socket will
 * be created if SOCK_DGRAM is set.

 *
 * @note 
 * 1. This function must be called after init_sock_environment being called. 
 * 2. Socket is set to be non-blocking socket by default. Call cssock_block explicitly if you need the socket to be blocking.
 *
 * @sa cssock_block cssock_open
 */
void csclient_init(struct csclient* cli, int tcpudp);

void csclient_connect(struct csclient* cli, const struct sockaddr* servaddr, int servaddr_len);

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
 * @brief  csclient_communicate is the interface to communicate with server.
 *
 * @param cli
 * @param fp is the FILE pointer where input data comes from.
 * @param serveraddr
 * @param serveraddr_len
 */
void csclient_communicate(struct csclient* cli, FILE* fp, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len);

/**
 * @brief  csclient_sendrecv 
 *
 * @param cli
 * @param serveraddr
 * @param serveraddr_len
 *
 * @return   
 */
ssize_t csclient_sendrecv(struct csclient* cli, const struct sockaddr* serveraddr, cssocklen_t serveraddr_len);


#ifdef __cplusplus
}
#endif


#endif  // CLIENT_UDP_H

