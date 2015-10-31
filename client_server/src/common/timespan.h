/**
 * @file timespan.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Sat 2015-10-31 19:10:43 (+0800)
 */

#ifndef _TIMESPAN_H
#define _TIMESPAN_H

#ifdef WIN32
#include  <Windows.h>
#define timelong_t ULARGE_INTEGER
#else
#include  <sys/time.h>
#include  <linux/errno.h>
#define timelong_t struct timeval
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void csgettimeofday(struct timeval* tv, void* tz);
void get_cur_timelong(timelong_t* tl);
unsigned long long get_span_microsec(const timelong_t* start);
unsigned int get_span_millisec(const timelong_t* start);
unsigned int get_span_sec(const timelong_t* start);

#ifdef __cplusplus
}
#endif

#endif //TIMESPAN_H
