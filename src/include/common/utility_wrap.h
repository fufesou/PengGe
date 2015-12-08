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
#define csprintf sprintf_s
#else
#define csprintf snprintf
#endif


/**
 * @brief  cs_memecpy cs_memcpy is a simple wrapper of memcpy_s(windows) or memcpy(unix).
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
CS_API int cs_memcpy(void* dst, size_t dstsize, const void* src, size_t count);

/**
 * @brief cs_fopen
 * @param streamptr
 * @param filename
 * @param mode
 * @return 0 if succeed, 1 if fail.
 *
 * @todo move this function to common lib.
 */
CS_API int cs_fopen(FILE** streamptr, const char* filename, const char* mode);


#ifndef WIN32
typedef void cssigfunc(int);

/**
 * @brief  cssignal cssignal works the same as signal.
 *
 * @param signo
 * @param func
 *
 * @return   
 */
cssigfunc* cssignal(int signo, cssigfunc* func);

/**
 * @brief  cssignal_ext works the same as cssignal, but to print error message
 * and exit if func fail to register.
 *
 * @param signo
 * @param func
 *
 * @return   
 */
cssigfunc* cssignal_ext(int signo, cssigfunc* func);
#endif

#ifdef __cplusplus
}
#endif

#endif //UTILITY_WRAP_H