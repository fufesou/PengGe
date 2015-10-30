/**
 * @file clien_sendrecv.h
 * @brief This is a wrap of sendmsg(linux) or WSASendTo(windows).
 * @author cxl
 * @version 0.1
 * @date 2015-10-03
 */

#ifndef  CLIENT_SENDRECV_H
#define  CLIENT_SENDRECV_H

#include    "macros.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  cssend 
 *
 * @param hsock
 * @param outbuf
 * @param outbytes
 * @param destaddr
 * @param destlen
 *
 * @return   
 */
ssize_t cssend(cssock_t hsock, const void* outbuf, size_t outbytes, const struct sockaddr* destaddr, int destlen);

/**
 * @brief  csrecv 
 *
 * @param hsock
 * @param inbuf
 * @param inbytes
 * @param destaddr
 * @param destlen
 *
 * @return   
 */
ssize_t csrecv(cssock_t hsock, void* inbuf, __inout size_t* inbytes, const struct sockaddr* destaddr, int destlen);

/**
 * @brief  cssendrecv 
 *
 * @param hsock
 * @param outbuf
 * @param outbytes
 * @param inbuf
 * @param inbytes
 * @param destaddr
 * @param destlen
 *
 * @return   
 */
ssize_t cssendrecv(cssock_t hsock, const void* outbuf, size_t outbytes, void* inbuf, size_t inbytes, const struct sockaddr* destaddr, int destlen);


#ifdef __cplusplus
}
#endif


#endif  // CLIENT_SENDRECV_H

