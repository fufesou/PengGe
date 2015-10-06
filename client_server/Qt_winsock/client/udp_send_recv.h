/**
 * @file udp_send_recv.h
 * @brief This is a wrap of sendmsg(linux) or WSASendTo.
 * @author cxl
 * @version 0.1
 * @date 2015-10-03
 */


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
ssize_t U_send_recv(SOCKET fd, const void* outbuf, size_t outbytes, void* inbuf, size_t inbytes, const SOCKETADDR* destaddr, int destlen);
#else
ssize_t U_send_recv(int fd, const void* outbuf, size_t outbytes, void* inbuf, size_t inbytes, const struct sockaddr* destaddr, int destlen);
#endif


#ifdef __cplusplus
}
#endif
