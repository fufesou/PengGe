/**
 * @file timespan.c
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Mon 2015-10-26 19:37:11 (+0800)
 */

#ifdef __cplusplus
extern "C"
{
#endif

static void s_get_cur_time(timelong_t* tl);

#ifdef __cplusplus
}
#endif


void s_get_cur_time(timelong_t* tl)
{
#ifdef WIN32
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	tl->HightPart = ft.dwHightDateTime;
	tl->LowPart = ft.dwLowDateTime;
#else
	gettimeofday(tl, NULL);
#endif
}

void reset_timelong(timelong_t* tl)
{
	s_get_cur_time(tl);
}

unsigned long long get_span_microsec(const timelong_t* start);
{
	timelong_t end;
	s_get_cur_time(&end);

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

