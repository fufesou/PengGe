/**
 * @file jxiot.h
 * @brief  This file define the commonly used consts variables and macros in client-server.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-29
 * @modified  Wed 2016-01-06 22:50:45 (+0800)
 */

// #define _CHECK_ARGS

#ifndef _JXIOT_H
#define _JXIOT_H

/** @defgroup jx_macros
 *
 * @{
 */

/** @defgroup utility_macros
 *
 * @{
 */

#define SIZEOF_ARR(arr) (sizeof(arr)/sizeof((arr)[0]))

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifndef container_of
/**
 * @breif container_of - cast a member of a structure out to the containing structure
 * @param ptr    the pointer to the member.
 * @param type   the type of the container struct this is embedded in.
 * @param member the name of the member within the struct.
 *
 * @note msvc2013 seems not to support 'typeof()'.
 */
#ifdef _MSC_VER
#define container_of(ptr, type, member) ((type *)((char *)ptr - offsetof(type, member)))
#else
#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member)*__mptr = (ptr);    \
             (type *)((char *)__mptr - offsetof(type, member)); })
#endif
#endif

/**
 * @breif TO_MULTIPLE_OF returns the closest multiple number of 'base' that is greater than 'num'.
 * @param num must be integer type.
 * @param base must be exponent of 2.
 *
 */
#define TO_MULTIPLE_OF(num, base) \
    (((num)+base-1) & (~(base-1)))

/** @} */


/** @defgroup func_macros
 *
 * @{
 */

#ifndef __jxin
#define __jxin
#endif

#ifndef __jxout
#define __jxout
#endif

#ifndef __jxinout
#define __jxinout
#endif

#ifdef WIN32
#define JXIOT_CALLBACK __cdecl
#else                   /* *nix */
#define JXIOT_CALLBACK
#endif                  /* WIN32 */

#ifdef WIN32
#ifdef PGCS_DLL
#ifdef PGCS_BUILDING_LIB
#define JXIOT_API __declspec( dllexport )
#else
#define JXIOT_API __declspec( dllimport )
#endif                  /* PGCS_BUILDING_LIB */
#else                   /* !PGCS_DLL */
#define JXIOT_API extern
#endif                  /* PGCS_DLL */
#else
#define JXIOT_API extern   /* *nix */
#endif                  /* WIN32 */

/** @} */


/** @defgroup mflag definitions
 * 'mflag' is the message flag, which is also the first byte of message.
 *
 * @{
*/

/** @defgroup mflag_s definitions
 * @ingroup mflag
 * mflag_s indicates message sessions begin with server.
 * mflag_s uses the last 4 bits of 'mflag', the left 4 bits must be 0.
 *
 * @{
 */
#define JX_MFLAG_SERVER_QUERY          0x00    /**< This message is the query from server that will be executed by client */
#define JX_MFLAG_SERVER_NOTIFY         0x01    /**< This message is just plain notification message that will be shown in client ui. */
#define JX_MFLAG_SERVER_LOG            0x02    /**< This message is just plain notification message that will be logged in client's log. */
/** @} */

/** @defgroup mflag_c definitions
 * @ingroup mflag
 * mflag_c indicates message sessions begin with client.
 * mflag_c uses all bits except collections of mflag_s.
 *
 * @{
 */
#define JX_MFLAG_CLIENT_QUERY          0x10    /**< This message will be treated as query in server. */
#define JX_MFLAG_CLIENT_NOTIFY         0x11    /**< This message will be shown in server's ui. */
#define JX_MFLAG_CLIENT_LOG            0x12    /**< This message will be shown in server's log. */
#define JX_MFLAG_CLIENT_COMMUNICATE_C  0x20    /**< This message will be delivered to another client. */
#define JX_MFLAG_CLIENT_COMMUNICATE_S  0x21    /**< This message will be shown in server's ui. */
#define JX_MFLAG_CLIENT_P2P            0x30    /**< This message will be regard as a 'p2p' reqeust message. */
/** @} */

/** These mflags indicate whether current message session begins with server(or client) 
 *
 * The last 4 bits are reserved for server.
 * */
#define JX_MFLAG_BEGIN_SERVER(f) ((f) < 0x10)
#define JX_MFLAG_BEGIN_CLIENT(f) ((f) >= 0x10)

#define JX_MFLAG_WAIT_RECV(f) ((f) == JX_MFLAG_CLIENT_QUERY)

/** @} */


/** @defgroup jxerrno the error number definistions
 *
 * @{
 */

#define JX_NO_ERR           (0)
#define JX_WARNING        	(1)
#define JX_NORMAL_ERR      	(2)
#define JX_FATAL           	(3)
#define JX_INVALIE_ARGS     (-1)
#define JX_UNKOWN_ERR       (-2)

/** @} */


/** @} */

#endif //JXIOT_H
