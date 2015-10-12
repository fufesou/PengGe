/**
 * @file client_udp.h
 * @brief This file provide the interfaces of udp client.
 * @author cxl
 * @version 0.1
 * @date 2015-09-30
 */

#ifndef  CLIENT_UDP_H
#define  CLIENT_UDP_H

#include <stdio.h>
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * @brief struct client_udp contains necessary members and functions for client operations.
     * This struct object must be initialized (by calling init_client_udp) before any operations applied to it.
     * @sa init_client_udp
     */
struct client_udp
{
    SOCKET socket;

    /**
     * @brief sockaddr_in is the server socket address.
     */
    SOCKADDR_IN sockaddr_in;

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

    /**
     * @brief  set_socket create socket for client.
     * @param client_udp
     * @note only AF_INET is supported by now
     */
    void (*set_socket)(struct client_udp* cli_udp);

    /**
     * @brief dg_client is the interface to communicate with server.
     *
     * @param cli_udp
     * @param fp is the FILE pointer where input data comes from.
     * @param serveraddr
     * @param serveraddr_len
     */
    void (*dg_client)(struct client_udp* cli_udp, FILE* fp, const struct sockaddr* serveraddr, int serveraddr_len);

    /**
     * @brief clear will do some clear tasks, such as closing socket.
     *
     * @param  cli_udp
     */
    void (*clear)(struct client_udp* cli_udp);
};

/**
 * @brief check_args checks whether the arguments are correct. If invalid paramters are passed, this function will give some prompts.
 *
 * @param argc
 * @param argv[]
 */
void check_args(int argc, char* argv[]);

/**
 * @brief init_client_udp sets basic members and function pointers to the client object.
 *
 * @param cli_udp
 *
 * @note This function must be called immediately after WSAStartup being called.
 */
void init_client_udp(struct client_udp *cli_udp);


#ifdef __cplusplus
}
#endif


#endif  // CLIENT_UDP_H

