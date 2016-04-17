/**
 * @file timespan.c
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Sun 2015-12-06 18:20:52 (+0800)
 */

#ifdef WIN32
#include  <time.h>
#include  <windows.h>
#else
#include  <sys/time.h>
#include  <linux/errno.h>
#endif

#include  <stdio.h>
#include    "common/timespan.h"

void jxgettimeofday(struct timeval* tv, void* tz)
{
#ifdef WIN32
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    (void)tz;

    GetLocalTime(&wtm);
    tm.tm_year    = wtm.wYear - 1900;
    tm.tm_mon     = wtm.wMonth - 1;
    tm.tm_mday    = wtm.wDay;
    tm.tm_hour    = wtm.wHour;
    tm.tm_min     = wtm.wMinute;
    tm.tm_sec     = wtm.wSecond;
    tm. tm_isdst  = -1;
    clock = mktime(&tm);
    tv->tv_sec = (long)clock;
    tv->tv_usec = wtm.wMilliseconds * 1000;
#else
    gettimeofday(tv, tz);
#endif
}

void jxtimelong_cur(jxtimelong_t* tl)
{
#ifdef WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    tl->HighPart = ft.dwHighDateTime;
    tl->LowPart = ft.dwLowDateTime;
#else
    gettimeofday(tl, NULL);
#endif
}

unsigned long long jxtimelong_span_microsec(const jxtimelong_t* start)
{
    jxtimelong_t end;
    jxtimelong_cur(&end);

#ifdef WIN32
    return (end.QuadPart - start->QuadPart) / 10;
#else
    return 1000000LL * (end.tv_sec - start->tv_sec) + end.tv_usec - start->tv_usec;
#endif
}

unsigned int jxtimelong_span_millisec(const jxtimelong_t* start)
{
    return (unsigned int)(jxtimelong_span_microsec(start) / 1000LL);
}

unsigned int jxtimelong_span_sec(const jxtimelong_t* start)
{
    return (unsigned int)(jxtimelong_span_microsec(start) / 1000000LL);
}

