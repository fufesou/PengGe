/**
 * @file sock_wrap.h
 * @brief This file defines some basic wrapper functions of socket.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-20
 * @modified  Fri 2016-01-01 18:32:31 (+0800)
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
CS_API int cssock_envinit(void);

/**
 * @brief  cssock_envclear will do basic clear works for sock environment.
 *
 * The parameter is just used for common prototype.
 *
 */
CS_API void cssock_envclear(void* unused);

/**
 * @brief  cssock_get_last_error will return last error number.
 *
 * @note This function may not be thread-safe.
 *
 * @return  The last error number. 
 */
CS_API int cssock_get_last_error(void);

/**
 * @brief  csaddrin_set 
 *
 * @param addr_in
 * @param ip
 * @param port
 */
CS_API void csaddrin_set(struct sockaddr_in* addr_in, const char* ip, int port);

/**
 * @brief  cssock_open cssock_open will create a socket handle according to tcpudp option.
 *
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set, upd socket will
 * be created if SOCK_DGRAM is set.
 *
 * @return   
 */
CS_API cssock_t cssock_open(int tcpudp);

/**
 * @brief  cssock_close 
 *
 * @param handle
 */
CS_API void cssock_close(cssock_t handle);

CS_API void cssock_connect(cssock_t handle, const struct sockaddr* sa, cssocklen_t addrlen);

CS_API void cssock_bind(cssock_t handle, struct sockaddr* sa, cssocklen_t addrlen);

CS_API void cssock_listen(cssock_t handle, int maxconn);

CS_API cssock_t cssock_accept(cssock_t handle, const struct sockaddr* sa, cssocklen_t* addrlen);

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
CS_API int cssock_block(cssock_t handle, int block);

/**
 * @brief  cssock_print print the IP and port for current socket handle, if socket is connected.
 *
 * @param handle specified socket to be test and print.
 * @param header specify the header message of print infomation.
 *
 * @return   
 * 1. JX_INVALID_ARGS(-1), if socket is not a valid socket.
 * 2. JX_NO_ERR(0), if socket is connected.
 * 3. JX_WARNING(1), if socket is not connected.
 */
CS_API int cssock_print(cssock_t handle, const char* header);

/**
 * @brief cssock_getsockname
 * @param handle
 * @param addr
 * @param addrlen
 * @return
 */
CS_API int cssock_getsockname(cssock_t handle, struct sockaddr* addr, cssocklen_t* addrlen);

/**
 * @brief cssock_getpeername
 * @param handle
 * @param addr
 * @param addrlen
 * @return
 */
CS_API int cssock_getpeername(cssock_t handle, struct sockaddr* addr, cssocklen_t* addrlen);

#ifdef WIN32
const char* cssock_inet_ntop(int af, const void* src, char* dst, cssocklen_t size);
int cssock_inet_pton(int af, const char* src, void* dst);
#else
#define cssock_inet_ntop inet_ntop
#define cssock_inet_pton inet_pton
#endif


#ifdef __cplusplus
}
#endif


#endif  // SOCK_WRAP_H
