/**
 * @file error.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-24
 * @modified  2015-10-24 17:23:23 (����)
 */

#include  <stdlib.h>
#include  <stdint.h>
#include    "error.h"
#include    "sock_wrap.h"

#ifdef __cplusplus
extern "C"
{
#endif

void s_error_operation(void* data, enum error_op errop);

#ifdef __cplusplus
}
#endif

void fatal(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    clear_sock_environment();
    exit(0);
}

void fatalext(void* data, enum error_op errop, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    s_error_operation(data, errop);
}

void s_error_operation(void* data, enum error_op errop)
{
    int willexit = errop & err_exit;

    if (errop == err_exit) {
        clear_sock_environment();
        exit(*(int*)data);
    }

    errop &= (~err_exit);
    switch (errop) {
        case err_clear:
            ((void (*)(void))data)();
            break;

        default:
            break;
    }
    if (willexit) {
        clear_sock_environment();
        exit(0);
    }
}

void warning(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
}

void warning_ext(void* data, enum error_op errop, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);

    s_error_operation(data, errop);
}
