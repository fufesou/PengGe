/**
 * @file error.h
 * @brief  This file defines some crude error handlers.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-24
 * @modified  Tue 2016-01-05 22:54:09 (+0800)
 */

#ifndef _ERROR_H
#define _ERROR_H

typedef size_t jxerr_t;
typedef void (JXIOT_CALLBACK *perr_clear_func_t)(void);

#ifdef __cplusplus
extern "C"
{
#endif

enum jxerr_op
{
    jxerr_exit  = 0x0001,
    jxerr_clear = 0x0002,
};

/**
 * @brief  jxfatal Print jxfatal error message and exit(0)
 *
 * @param format
 * @param ...
 */
void jxfatal(const char* format, ...);

/**
 * @brief  jxfatal_ext This function will print error message, clear socket
 * environment and exit.
 * If errop is jxerr_exit, exit(*(jxerr_t*)data) will be called.
 * if errop is jxerr_clear, ((perr_clear_func_t)data)() will be called.
 *
 * @param data 
 * @param errop
 * @param format
 * @param ...
 */
void jxfatal_ext(void* data, enum jxerr_op errop, const char* format, ...);

/**
 * @brief  jxwarning 
 *
 * @param format
 * @param ...
 *
 * @sa jxfatal
 *
 * @todo use macros(contains __func__, __file__, __line__) instead of functions.
 */
void jxwarning(const char* format, ...);

/**
 * @brief  jxwarning_ext 
 *
 * @param data
 * @param errop
 * @param format
 * @param ...
 *
 * @sa jxfatal_ext
 */
void jxwarning_ext(void* data, enum jxerr_op errop, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif //ERROR_H
