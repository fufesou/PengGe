/**
 * @file clearlist.h
 * @brief  This file defines a list that contains various clear functions for 
 * program to safely exit.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-19
 * @modified  Sun 2015-12-06 18:24:37 (+0800)
 */

#ifndef _CLEARLIST_H
#define _CLEARLIST_H

struct clear_item_t {
    void (JXIOT_CALLBACK *pfunc_clear)(void*);
    void* pargs;
};

struct list_clear_t {
    struct list_head listnode;
    struct clear_item_t clear_item;
};

#ifdef __cplusplus
extern "C" {
#endif

JXIOT_API void jxclearlist_add(void (JXIOT_CALLBACK *pfunc_clear)(void* pargs), void* pargs);
JXIOT_API void jxclearlist_clear();

#ifdef __cplusplus
}
#endif


#endif //CLEARLIST_H
