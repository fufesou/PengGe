/**
 * @file error.c
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-24
 * @modified  Fri 2015-11-20 00:24:58 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <netinet/in.h>
#include  <sys/socket.h>
#endif

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/error.h"
#include    "common/list.h"
#include    "common/sock_types.h"
#include    "common/sock_wrap.h"
#include    "common/clearlist.h"

#ifdef __cplusplus
extern "C"
{
#endif

void s_jxerr_operation(void* data, enum jxerr_op errop);

#ifdef __cplusplus
}
#endif

void jxfatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    jxclearlist_clear();
    exit(0);
}

void jxfatal_ext(void* data, enum jxerr_op errop, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    s_jxerr_operation(data, errop);
}

void s_jxerr_operation(void* data, enum jxerr_op errop)
{
    int willexit = errop & jxerr_exit;

    if (errop == jxerr_exit) {
        jxclearlist_clear();
        exit((int)*(jxerr_t*)data);
    }

    errop &= (~jxerr_exit);
    switch (errop) {
        case jxerr_clear:
            ((perr_clear_func_t)data)();
            break;

        default:
            break;
    }
    if (willexit) {
        jxclearlist_clear();
        exit(0);
    }
}

void jxwarning(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void jxwarning_ext(void* data, enum jxerr_op errop, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    s_jxerr_operation(data, errop);
}
