/**
 * @file sock_types.h
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-10
 * @modified  2015-10-24 16:46:06 (周六)
 */


#ifndef  UDP_TYPES_H
#define  UDP_TYPES_H


/**
 * @par platform macros
 *
 */
#ifdef WIN32
#else
#define SOCKET_ERROR (-1)
#define INVALID_SOCKET 0
#endif


/**
 * @par  platform types
 *
 */
#ifdef WIN32
typedef SOCKET sock_t;
#else
typedef int sock_t;
#endif



/**
 * @par  platform independence structures
 *
 */
struct hdr {
    uint32_t seg;
    uint32_t ts;
};

struct sock_option
{
    int block;
    int size_sendbuf;
    int size_recvbuf;
};

#endif  // UDP_TYPES_H
