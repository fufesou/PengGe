/**
 * @file sock_wrap.h
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 * @modified  Sat 2015-10-31 15:04:38 (+0800)
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
 * @brief  cssock_envinit must be called before socket created. Similarly, cssock_envclear must be called before exit();
 *
 * @return
 *
 * @see cssock_envclear
 */
int cssock_envinit(void);

/**
 * @brief  cssock_envclear 
 */
void cssock_envclear(void);

/**
 * @brief  cssock_get_last_error will return last error number.
 *
 * @note This function may not be thread-safe.
 *
 * @return  The last error number. 
 */
int cssock_get_last_error(void);

/**
 * @brief  csaddrin_set 
 *
 * @param addr_in
 * @param ip
 * @param port
 */
void csaddrin_set(struct sockaddr_in* addr_in, const char* ip, int port);

/**
 * @brief  cssock_open cssock_open will create a socket handle according to tcpudp option.
 *
 * @param tcpudp tcp socket will be created if SOCK_STREAM  is set, upd socket will
 * be created if SOCK_DGRAM is set.
 *
 * @return   
 */
cssock_t cssock_open(int tcpudp);

/**
 * @brief  cssock_close 
 *
 * @param handle
 */
void cssock_close(cssock_t handle);

/*
void cssock_connect(cssock_t handle, const struct sockaddr_in* sa_in);

void cssock_accept(cssock_t handle, const struct sockaddr_in* sa_in);
*/

/**
 * @brief  cssock_block cssock_block will set socket to be blocking or nonblocking according to block parameter.
 *
 * @param handle The socket handle.
 * @param block The blocking or nonblocking option.
 *
 * @return   
 * 1. block set result, if handle is a valid socket.
 * 2. -1, if handle is not a valid socket.
 */
int cssock_block(cssock_t handle, int block);

/**
 * @brief  cssock_print print the IP and port for current socket handle, if socket is connected.
 *
 * @param handle specified socket to be test and print.
 * @param header specify the header message of print infomation.
 *
 * @return   
 * 1. -1, if socket is not a valid socket.
 * 2. 0, if socket is connected.
 * 3. 1, if socket is not connected.
 */
int cssock_print(cssock_t handle, const char* header);

#ifdef __cplusplus
}
#endif


#endif  // SOCK_WRAP_H
