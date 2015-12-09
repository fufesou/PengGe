/**
 * @file guimacros.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Wed 2015-12-09 20:45:47 (+0800)
 */

#ifndef _GUIMACROS_H
#define _GUIMACROS_H

#define SAFE_DEL(p) { if (p) { delete (p); p = NULL; } } 
#define SAFE_ARR_DEL(p) { if (p) { delete [] (p); p = NULL; } } 

#ifndef NUM_ARR
#define NUM_ARR(arr) (sizeof(arr) / sizeof(arr[0]))
#endif 

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif


#endif //GUIMACROS_H
