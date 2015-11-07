/**
 * @file timespan.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Sat 2015-10-31 19:10:43 (+0800)
 */

#ifndef _TIMESPAN_H
#define _TIMESPAN_H

#ifdef WIN32
#include  <Windows.h>
#define cstimelong_t ULARGE_INTEGER
#else
#include  <sys/time.h>
#include  <linux/errno.h>
#define cstimelong_t struct timeval
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void csgettimeofday(struct timeval* tv, void* tz);
void cstimelong_cur(cstimelong_t* tl);
unsigned long long cstimelong_span_microsec(const cstimelong_t* start);
unsigned int cstimelong_span_millisec(const cstimelong_t* start);
unsigned int cstimelong_span_sec(const cstimelong_t* start);

#ifdef __cplusplus
}
#endif

#endif //TIMESPAN_H
