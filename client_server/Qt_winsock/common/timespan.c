/**
 * @file timespan.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  2015-10-26 23:15:37 (+0800)
 */

#include	"timespan.h"
#include  <stdio.h>

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

