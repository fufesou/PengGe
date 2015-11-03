/**
 * @file server_helper.h
 * @brief This file provide some helper functions for server.
 * @author cxl
 * @version 0.1
 * @date 2015-09-26
 * @modified  Tue 2015-11-03 19:35:34 (+0800)
 */

#ifndef  SERVER_HELPER_H
#define  SERVER_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif


struct csserver
{
    cssock_t hsock;
    struct sockaddr_in sa_in;
    char* msgheader;
};

/**
 * @brief  csserver_init 
 *
 * @param serv
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set, upd socket will be created if SOCK_DGRAM is set.
 *
 * @note This function must be called after init_sock_environment being called. 
 */
void csserver_init(struct csserver* serv, int tcpudp, u_short port, u_long addr);

int csserver_print(struct csserver* serv);

void csserver_udp(struct csserver* serv);


#ifdef __cplusplus
}
#endif

#endif  // SERVER_HELPER_H
