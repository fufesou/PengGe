/**
 * @file utility_wrap.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-29
 * @modified  Sat 2015-10-31 17:49:03 (+0800)
 */

#ifndef _UTILITY_WRAP_H
#define _UTILITY_WRAP_H

#ifdef __cplusplus
extern "C"
{
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
 * -1 if dstsize < count.
 *  1 if (src == NULL || dst == NULL).
 *  0 if success.
 *
 *  @note c11 has introduced memcpy_s.
 */
int cs_memcpy(void* dst, size_t dstsize, const void* src, size_t count);


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