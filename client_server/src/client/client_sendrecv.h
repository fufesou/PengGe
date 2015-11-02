/**
 * @file clien_sendrecv.h
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-03
 * @modified  Mon 2015-11-02 19:04:07 (+0800)
 */

#ifndef  CLIENT_SENDRECV_H
#define  CLIENT_SENDRECV_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief  cssendrecv 
 *
 * @param hsock hsock is the socket handle.
 * @param rttinfo
 * @param outmsg
 * @param outbytes outbytes is the message length of outmsg.
 * @param inmsg
 * @param inbytes inbytes is the buffer size of inmsg.
 * @param destaddr
 * @param destlen
 *
 * @return   
 * 1. The length of received message.
 * 2. -1, if timeout.
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

