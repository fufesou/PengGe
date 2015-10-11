/**
 * @file server_sendrecv.h
 * @brief 
 * @author cxl
 * @version 0.1
 * @date 2015-10-10
 */

#ifndef  SERVER_SENDRECV_H
#define  SERVER_SENDRECV_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
ssize_t server_recv(
        SOCKET fd,
        void* inbuf,
        size_t inbytes,
        struct hdr* hdrdata,
        struct sockaddr* cli_addr,
        int* cli_addrlen);

void server_send(
        SOCKET fd,
        const void* outbuf,
        size_t outbytes,
        struct hdr* hdrdata,
        const struct sockaddr* cli_addr,
        int cli_addrlen);
#else
ssize_t server_recv(int fd, void* inbuf, size_t inbytes, void* verifydata, int* verifylen, struct sockaddr* cli_addr, int* cli_addrlen);
ssize_t server_send(int fd, const void* outbuf, size_t outbytes, const void* verifydata, int verifylen, const struct sockaddr* cli_addr, int cli_addrlen);
#endif


#ifdef __cplusplus
}
#endif


#endif  // SERVER_SENDRECV_H
