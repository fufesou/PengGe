/**
 * @file guimacros.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-12-03
 * @modified  周四 2015-12-03 21:44:02 中国标准时间
 */

#ifndef _GUIMACROS_H
#define _GUIMACROS_H

#define SAFE_DEL(p) { if (p) { delete (p); p = NULL; } } 
#define SAFE_ARR_DEL(p) { if (p) { delete [] (p); p = NULL; } } 
#define NUM_ARR(arr) (sizeof(arr) / sizeof(arr[0]))


#endif //GUIMACROS_H
