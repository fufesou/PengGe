/**
 * @file utility_wrap.h
 * @brief  This function defines some utility wrap functions, such as memcpy.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-29
 * @modified  Thu 2015-11-19 19:22:20 (+0800)
 */

#ifndef _UTILITY_WRAP_H
#define _UTILITY_WRAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
#define jxsprintf sprintf_s
#else
#define jxsprintf snprintf
#endif


/**
 * @brief  jx_memecpy jxmemcpy is a simple wrapper of memcpy_s(windows) or memcpy(unix).
 *
 * @param dst
 * @param dstsize
 * @param src
 * @param count
 *
 * @return   
 *  - 0 if succeed.
 *  - -1 if argument is invalid, (dstsize < count) || (src == NULL) || (dst == NULL).
 *
 *  @note c11 has introduced memcpy_s.
 */
JXIOT_API int jxmemcpy(void* dst, size_t dstsize, const void* src, size_t count);

/**
 * @brief jxfopen
 * @param streamptr
 * @param filename
 * @param mode
 * @return 0 if succeed, 1 if fail.
 *
 * @todo move this function to common lib.
 */
JXIOT_API int jxfopen(FILE** streamptr, const char* filename, const char* mode);


#ifndef WIN32
typedef void jxsigfunc(int);

/**
 * @brief  jxsignal jxsignal works the same as signal.
 *
 * @param signo
 * @param func
 *
 * @return   
 */
jxsigfunc* jxsignal(int signo, jxsigfunc* func);

/**
 * @brief  jxsignal_ext works the same as jxsignal, but to print error message
 * and exit if func fail to register.
 *
 * @param signo
 * @param func
 *
 * @return   
 */
jxsigfunc* jxsignal_ext(int signo, jxsigfunc* func);
#endif

#ifdef __cplusplus
}
#endif

#endif //UTILITY_WRAP_H
