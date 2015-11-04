/**
 * @file sock_types.h
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-10
 * @modified  Tue 2015-11-03 18:10:07 (+0800)
 */


#ifndef  SOCK_TYPES_H
#define  SOCK_TYPES_H

#ifdef WIN32
#define IS_SOCK_HANDLE(x) ((x)!=INVALID_SOCKET)
#else
#define IS_SOCK_HANDLE(x) ((x)>0)
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
    uint32_t seq;
    uint32_t ts;
};

struct sock_option
{
    int block;
    int size_sendbuf;
    int size_recvbuf;
};

#endif  // SOCK_TYPES_H
