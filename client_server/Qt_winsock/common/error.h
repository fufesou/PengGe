/**
 * @file error.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-24
 * @modified  2015-10-24 17:09:28 (÷‹¡˘)
 */

#ifndef _ERROR_H
#define _ERROR_H

#include  <stdarg.h>
#include  <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

enum error_op
{
    err_exit  = 0x0001,
    err_clear = 0x0002,
};

/**
 * @brief  fatal Print fatal error message and exit(0)
 *
 * @param format
 * @param ...
 */
void fatal(const char* format, ...);

/**
 * @brief  fatal_ext 
 *
 * @param data
 * @param errop
 * @param format
 * @param ...
 */
void fatal_ext(void* data, enum error_op errop, const char* format, ...);

/**
 * @brief  warning 
 *
 * @param format
 * @param ...
 */
void warning(const char* format, ...);

/**
 * @brief  warning_ext 
 *
 * @param data
 * @param errop
 * @param format
 * @param ...
 */
void warning_ext(void* data, enum error_op errop, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif //ERROR_H
