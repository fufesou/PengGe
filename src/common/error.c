/**
 * @file error.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
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
#include    "cstypes.h"
#include    "error.h"
#include    "list.h"
#include    "sock_types.h"
#include    "sock_wrap.h"
#include    "clearlist.h"

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

    csclearlist_clear();
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
        csclearlist_clear();
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
        csclearlist_clear();
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
