/**
 * @file macros.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-16
 */

#ifndef  MACROS_H
#define  MACROS_H

// #define CHECK_PARAM


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
 */
#define container_of(ptr, type, member) ({          \
	const typeof(((type *)0)->member)*__mptr = (ptr);    \
		     (type *)((char *)__mptr - offsetof(type, member)); })
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

