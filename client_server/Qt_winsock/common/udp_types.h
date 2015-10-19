/**
 * @file udp_types.h
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-10
 */


#ifndef  UDP_TYPES_H
#define  UDP_TYPES_H


#ifdef WIN32
struct WSASendRecvMsg {
    LPWSABUF msg_iov;
    DWORD msg_iovlen;
    DWORD numbytes;
    DWORD flags;
    struct sockaddr_in msg_name;
    int msg_namelen;
};
#endif

struct hdr {
    uint32_t seg;
    uint32_t ts;
};


#endif  // UDP_TYPES_H
