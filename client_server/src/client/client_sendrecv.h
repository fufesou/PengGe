/**
 * @file clien_sendrecv.h
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-03
 * @modified  Sat 2015-10-31 18:44:12 (+0800)
 */

#ifndef  CLIENT_SENDRECV_H
#define  CLIENT_SENDRECV_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  cssendrecv 
 *
 * @param hsock
 * @param rttinfo
 * @param outbuf
 * @param outbytes
 * @param inbuf
 * @param inbytes
 * @param destaddr
 * @param destlen
 *
 * @return   
 */
extern ssize_t cssendrecv(
			cssock_t hsock, __inout struct rtt_info* rttinfo,
			const void* outmsg, size_t outbytes,
			void* inmsg, size_t inbytes,
			const struct sockaddr* destaddr, cssocklen_t destlen);


#ifdef __cplusplus
}
#endif


#endif  // CLIENT_SENDRECV_H

