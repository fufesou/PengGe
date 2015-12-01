/**
 * @file cstypes.h
 * @brief This file defines common types.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-1
 * @modified  周二 2015-12-01 20:57:19 中国标准时间
 */

#ifndef  CS_TYPES_H
#define  CS_TYPES_H

#include  <limits.h>

#ifdef WIN32
#include  <windows.h>
#endif /* WIN32 */

#ifdef _MSC_VER /* WIN32 */

#if (_MSC_VER < 1300)
   typedef signed char       int8_t;
   typedef signed short      int16_t;
   typedef signed int        int32_t;
   typedef unsigned char     uint8_t;
   typedef unsigned short    uint16_t;
   typedef unsigned int      uint32_t;
#else
   typedef signed __int8     int8_t;
   typedef signed __int16    int16_t;
   typedef signed __int32    int32_t;
   typedef unsigned __int8   uint8_t;
   typedef unsigned __int16  uint16_t;
   typedef unsigned __int32  uint32_t;
#endif
typedef signed __int64       int64_t;
typedef unsigned __int64     uint64_t;
typedef SSIZE_T              ssize_t;

/********************************/

#else /* *nix */

/* C99 compiler has this head file */
#include <stdint.h>
/* do not nee this */
#if 0
/* part of stdint.h from GNU C Library */
/* __WORDSIZE defined in <bits/wordsize.h> */
#ifndef __int8_t_defined
# define __int8_t_defined
typedef signed char		int8_t;
typedef short int		int16_t;
typedef int			int32_t;
# if __WORDSIZE == 64
typedef long int		int64_t;
# else
__extension__
typedef long long int		int64_t;
# endif
#endif

typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int		uint32_t;
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
typedef unsigned long int	uint64_t;
#else
__extension__
typedef unsigned long long int	uint64_t;
#endif

#endif  /* #if 0 */

#endif /* _MSC_VER */

#endif  // CS_TYPES_H
