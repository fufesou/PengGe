/**
 * @file utility_wrap.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-29
 * @modified  Sun 2015-12-06 18:20:54 (+0800)
 */

#include  <memory.h>
#include  <stdio.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/error.h"
#include    "common/utility_wrap.h"

int jxmemcpy(void* dst, size_t dstsize, const void* src, size_t count)
{
    if (dstsize < count) {
        return -1;
    }

#ifdef _CHECK_ARGS
    if (dst == NULL || src == NULL) {
        return -1;
    }
#endif

#ifdef WIN32
    memcpy_s(dst, dstsize, src, count);
#else
    memcpy(dst, src, count);
#endif

    return 0;
}

int jxfopen(FILE** streamptr, const char* filename, const char* mode)
{
#ifdef WIN32
    if (fopen_s(streamptr, filename, mode) != 0) {
#else
    if (((*streamptr) = fopen(filename, mode)) == NULL) {
#endif
        /* printf("open %s error, error code: %d.\n", filename, ); */
        return 1;
    }
    return 0;
}

#ifndef WIN32
#include  <signal.h>

jxsigfunc* jxsignal(int signo, jxsigfunc* func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif
    }
    if (sigaction(signo, &act, &oact) < 0) {
        return (SIG_ERR);
    }
    return (oact.sa_handler);
}

jxsigfunc* jxsignal_ext(int signo, jxsigfunc* func)
{
    jxsigfunc* sigfunc;
    if ((sigfunc = jxsignal(signo, func)) == SIG_ERR) {
        jxfatal("signal error");
    }
    return sigfunc;
}

#endif
