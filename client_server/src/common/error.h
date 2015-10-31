/**
 * @file error.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-24
 * @modified  Sat 2015-10-31 12:13:12 (+0800)
 */

#ifndef _ERROR_H
#define _ERROR_H

#include  <stdarg.h>
#include  <stdio.h>

typedef size_t cserr_t;
typedef void (*cserr_clear_func)(void);

#ifdef __cplusplus
extern "C"
{
#endif

enum cserr_op
{
    cserr_exit  = 0x0001,
    cserr_clear = 0x0002,
};

/**
 * @brief  csfatal Print csfatal error message and exit(0)
 *
 * @param format
 * @param ...
 */
void csfatal(const char* format, ...);

/**
 * @brief  csfatal_ext This function will print error message, clear socket
 * environment and exit.
 * If errop is cserr_exit, exit(*(cserr_t*)data) will be called.
 * if errop is cserr_clear, ((cserr_clear_func)data)() will be called.
 *
 * @param data 
 * @param errop
 * @param format
 * @param ...
 */
void csfatal_ext(void* data, enum cserr_op errop, const char* format, ...);

/**
 * @brief  cswarning 
 *
 * @param format
 * @param ...
 *
 * @sa csfatal
 */
void cswarning(const char* format, ...);

/**
 * @brief  cswarning_ext 
 *
 * @param data
 * @param errop
 * @param format
 * @param ...
 *
 * @sa csfatal_ext
 */
void cswarning_ext(void* data, enum cserr_op errop, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif //ERROR_H
