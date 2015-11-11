/**
 * @file error.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-24
 * @modified  Tue 2015-11-03 19:30:14 (+0800)
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
#include  <stdint.h>
#include    "error.h"
#include    "sock_types.h"
#include    "sock_wrap.h"

#ifdef __cplusplus
extern "C"
{
#endif

void s_cserr_operation(void* data, enum cserr_op errop);

#ifdef __cplusplus
}
#endif

void csfatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    cssock_envclear();
    exit(0);
}

void csfatal_ext(void* data, enum cserr_op errop, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    s_cserr_operation(data, errop);
}

void s_cserr_operation(void* data, enum cserr_op errop)
{
    int willexit = errop & cserr_exit;

    if (errop == cserr_exit) {
        cssock_envclear();
        exit((int)*(cserr_t*)data);
    }

    errop &= (~cserr_exit);
    switch (errop) {
        case cserr_clear:
            ((cserr_clear_func)data)();
            break;

        default:
            break;
    }
    if (willexit) {
        cssock_envclear();
        exit(0);
    }
}

void cswarning(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void cswarning_ext(void* data, enum cserr_op errop, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    s_cserr_operation(data, errop);
}
