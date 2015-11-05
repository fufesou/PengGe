/**
 * @file sock_wrap.h
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-09-20
 * @modified  Fri 2015-11-06 01:06:43 (+0800)
 */

#ifndef  SOCK_WRAP_H
#define  SOCK_WRAP_H


#ifdef __cplusplus
extern "C" {
#endif


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
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set, upd socket will
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

void cssock_connect(cssock_t handle, const struct sockaddr* sa, cssocklen_t addrlen);

void cssock_bind(cssock_t handle, struct sockaddr* sa, cssocklen_t addrlen);

void cssock_listen(cssock_t handle, int maxconn);

cssock_t cssock_accept(cssock_t handle, const struct sockaddr* sa, cssocklen_t* addrlen);

/**
 * @brief  cssock_block cssock_block will set socket to be blocking or nonblocking according to block parameter.
 *
 * @param handle The socket handle.
 * @param block 
 * If block == 0, blocking is enabled. 
 * If block != 0, non-blocking is enalbed.
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

int cssock_getsockname(cssock_t handle, struct sockaddr* addr, cssocklen_t* addrlen);
int cssock_getpeername(cssock_t handle, struct sockaddr* addr, cssocklen_t* addrlen);


#ifdef __cplusplus
}
#endif


#endif  // SOCK_WRAP_H
