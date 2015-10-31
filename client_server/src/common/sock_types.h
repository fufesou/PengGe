/**
 * @file sock_types.h
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-10
 * @modified  Sat 2015-10-31 14:59:07 (+0800)
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
typedef SOCKET cssock_t;
typedef int cssocklen_t;
#else
typedef int cssock_t;
typedef socklen_t cssocklen_t;
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
