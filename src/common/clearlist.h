/**
 * @file clearlist.h
 * @brief  This file defines a list that contains various clear functions for 
 * program to safely exit.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-19
 * @modified  Fri 2015-11-20 00:07:02 (+0800)
 */

#ifndef _CLEARLIST_H
#define _CLEARLIST_H

struct clear_item_t {
	void (*pfunc_clear)(void*);
	void* pargs;
};

struct list_clear_t {
	struct list_head listnode;
	struct clear_item_t clear_item;
};

#ifdef __cplustplust
extern "C" {
#endif

void csclearlist_add(void (*pfunc_clear)(void* pargs), void* pargs);
void csclearlist_clear();

#ifdef __cplusplus
}
#endif


#endif //CLEARLIST_H
