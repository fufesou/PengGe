/**
 * @file timespan.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Sat 2015-10-31 19:13:32 (+0800)
 */

#ifdef WIN32
#include  <windows.h>
#else
#include  <sys/time.h>
#endif

#include	"timespan.h"
#include  <stdio.h>

void csgettimeofday(struct timeval* tv, void* tz)
{
#ifdef WIN32
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year    = wtm.wYear - 1900;
    tm.tm_mon     = wtm.wMonth - 1;
    tm.tm_mday    = wtm.wDay;
    tm.tm_hour    = wtm.wHour;
    tm.tm_min     = wtm.wMinute;
    tm.tm_sec     = wtm.wSecond;
    tm. tm_isdst  = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
#else
	gettimeofday(tv, tz);
#endif
}

void get_cur_timelong(timelong_t* tl)
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

unsigned long long get_span_microsec(const timelong_t* start)
{
	timelong_t end;
	get_cur_timelong(&end);

#ifdef WIN32
	return (end.QuadPart - start->QuadPart) / 10;
#else
	return 1000000LL * (end.tv_sec - start->tv_sec) + end.tv_usec - start->tv_usec;
#endif
}

unsigned int get_span_millisec(const timelong_t* start)
{
	return (unsigned int)(get_span_microsec(start) / 1000LL);
}

unsigned int get_span_sec(const timelong_t* start)
{
	return (unsigned int)(get_span_microsec(start) / 1000000LL);
}

