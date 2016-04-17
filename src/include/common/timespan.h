/**
 * @file timespan.h
 * @brief  This file defines "get time" functions and "time span" functions.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-26
 * @modified  Sat 2015-10-31 19:10:43 (+0800)
 */

#ifndef _TIMESPAN_H
#define _TIMESPAN_H

#ifdef WIN32
#define jxtimelong_t ULARGE_INTEGER
#else
#define jxtimelong_t struct timeval
#endif

#ifdef __cplusplus
extern "C"
{
#endif

void jxgettimeofday(struct timeval* tv, void* tz);
void jxtimelong_cur(jxtimelong_t* tl);
unsigned long long jxtimelong_span_microsec(const jxtimelong_t* start);
unsigned int jxtimelong_span_millisec(const jxtimelong_t* start);
unsigned int jxtimelong_span_sec(const jxtimelong_t* start);

#ifdef __cplusplus
}
#endif

#endif //TIMESPAN_H
