/**
 * @file macros.h
 * @brief  This file define some basic macro operations and functions.
 * And .. some macros stand for config data(recode this file after config parser done).
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-16
 * modified  周二 2015-11-10 19:05:56 中国标准时间
 */

#ifndef  MACROS_H
#define  MACROS_H

#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

#ifndef __inout
#define __inout
#endif

// #define _CHECK_ARGS

#define MAX_MSG_LEN 1024
#define MAX_BUF_LEN 2048

/*
 * @todo the following macros should be set by reading the config
 */
#define NUM_THREAD 8
#define SERVER_POOL_NUM_ITEM 64


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
#ifdef MSVC
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


#endif  // MACROS_H

