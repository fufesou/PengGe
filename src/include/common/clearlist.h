/**
 * @file clearlist.h
 * @brief  This file defines a list that contains various clear functions for 
 * program to safely exit.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-19
 * @modified  Sun 2015-12-06 18:24:37 (+0800)
 */

#ifndef _CLEARLIST_H
#define _CLEARLIST_H

struct clear_item_t {
    void (CS_CALLBACK *pfunc_clear)(void*);
    void* pargs;
};

struct list_clear_t {
    struct list_head listnode;
    struct clear_item_t clear_item;
};

#ifdef __cplustplust
extern "C" {
#endif

CS_API void csclearlist_add(void (CS_CALLBACK *pfunc_clear)(void* pargs), void* pargs);
CS_API void csclearlist_clear();

#ifdef __cplusplus
}
#endif


#endif //CLEARLIST_H
