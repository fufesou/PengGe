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
 * @brief  jxsock_envinit must be called before socket created. Similarly, jxsock_envclear must be called before exit();
 *
 * @return
 *
 * @see jxsock_envclear
 */
JXIOT_API int jxsock_envinit(void);

/**
 * @brief  jxsock_envclear will do basic clear works for sock environment.
 *
 * The parameter is just used for common prototype.
 *
 */
JXIOT_API void jxsock_envclear(void* unused);

/**
 * @brief  jxsock_get_last_error will return last error number.
 *
 * @note This function may not be thread-safe.
 *
 * @return  The last error number. 
 */
JXIOT_API int jxsock_get_last_error(void);

/**
 * @brief  jxaddrin_set
 *
 * @param addr_in
 * @param ip
 * @param port
 */
JXIOT_API void jxaddrin_set(struct sockaddr_in* addr_in, const char* ip, int port);

/**
 * @brief  jxsock_open jxsock_open will create a socket handle according to tcpudp option.
 *
 * @param tcpudp Tcp socket will be created if SOCK_STREAM  is set, upd socket will
 * be created if SOCK_DGRAM is set.
 *
 * @return   
 */
JXIOT_API jxsock_t jxsock_open(int tcpudp);

/**
 * @brief  jxsock_close
 *
 * @param handle
 */
JXIOT_API void jxsock_close(jxsock_t handle);

JXIOT_API void jxsock_connect(jxsock_t handle, const struct sockaddr* sa, jxsocklen_t addrlen);

JXIOT_API void jxsock_bind(jxsock_t handle, struct sockaddr* sa, jxsocklen_t addrlen);

JXIOT_API void jxsock_listen(jxsock_t handle, int maxconn);

JXIOT_API jxsock_t jxsock_accept(jxsock_t handle, const struct sockaddr* sa, jxsocklen_t* addrlen);

/**
 * @brief  jxsock_block jxsock_block will set socket to be blocking or nonblocking according to block parameter.
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
JXIOT_API int jxsock_block(jxsock_t handle, int block);

/**
 * @brief  jxsock_print print the IP and port for current socket handle, if socket is connected.
 *
 * @param handle specified socket to be test and print.
 * @param header specify the header message of print infomation.
 *
 * @return   
 * 1. JX_INVALID_ARGS(-1), if socket is not a valid socket.
 * 2. JX_NO_ERR(0), if socket is connected.
 * 3. JX_WARNING(1), if socket is not connected.
 */
JXIOT_API int jxsock_print(jxsock_t handle, const char* header);

/**
 * @brief jxsock_getsockname
 * @param handle
 * @param addr
 * @param addrlen
 * @return
 */
JXIOT_API int jxsock_getsockname(jxsock_t handle, struct sockaddr* addr, jxsocklen_t* addrlen);

/**
 * @brief jxsock_getpeername
 * @param handle
 * @param addr
 * @param addrlen
 * @return
 */
JXIOT_API int jxsock_getpeername(jxsock_t handle, struct sockaddr* addr, jxsocklen_t* addrlen);

#ifdef WIN32
const char* jxsock_inet_ntop(int af, const void* src, char* dst, jxsocklen_t size);
int jxsock_inet_pton(int af, const char* src, void* dst);
#else
#define jxsock_inet_ntop inet_ntop
#define jxsock_inet_pton inet_pton
#endif


#ifdef __cplusplus
}
#endif


#endif  // SOCK_WRAP_H
