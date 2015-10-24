/**
 * @file sock_wrap.h
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 * @modified  2015-10-24 18:30:18 (周六)
 */

#ifndef  SOCK_WRAP_H
#define  SOCK_WRAP_H


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <netinet/in.h>
#include  <sys/socket.h>
#include  <sys/types.h>
#endif

#include    "sock_types.h"


/**
 * @brief  init_sock_environment must be called before socket created. Similarly, clear_sock_environment must be called before exit();
 *
 * @see clear_sock_environment
 */
void init_sock_environment(void);

/**
 * @brief  clear_sock_environment 
 */
void clear_sock_environment(void);

/**
 * @brief  get_last_sock_error will return last error number.
 *
 * @note This function may not be thread-safe.
 *
 * @return  The last error number. 
 */
int get_last_sock_error(void);

/**
 * @brief  set_addrin 
 *
 * @param addr_in
 * @param ip
 * @param port
 */
void set_addrin(struct sockaddr_in* addr_in, const char* ip, int port);

/**
 * @brief  open_sock open_sock will create a socket handle according to tcpudp option.
 *
 * @param tcpudp
 *
 * @return   
 */
sock_t open_sock(int tcpudp);

/**
 * @brief  close_sock 
 *
 * @param handle
 */
void close_sock(sock_t handle);

/**
 * @brief  block_sock block_sock will set socket to be blocking or nonblocking according to block parameter.
 *
 * @param handle The socket handle.
 * @param block The blocking or nonblocking option.
 *
 * @return   
 * 1. block set result, if handle is a valid socket.
 * 2. -1, if handle is not a valid socket.
 */
int block_sock(sock_t handle, int block);

/**
 * @brief  print_sockinfo print the IP and port for current socket handle, if socket is connected.
 *
 * @param handle
 *
 * @return   
 * 1. -1, if socket is not a valid socket.
 * 2. 0, if socket is connected.
 * 3. 1, if socket is not connected.
 */
int print_sockinfo(sock_t handle);

#ifdef __cplusplus
}
#endif


#endif  // SOCK_WRAP_H
