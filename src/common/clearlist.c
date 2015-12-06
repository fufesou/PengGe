/**
 * @file clearlist.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-19
 * @modified  Sun 2015-12-06 17:44:34 (+0800)
 */

#include  <malloc.h>
#include  <stdlib.h>

#include    "common/macros.h"
#include    "common/list.h"
#include    "common/clearlist.h"

#ifdef __cplusplus
extern "C"
{
#endif

static LIST_HEAD(s_list_clear);

static void s_execute_clears(void);
static void s_clearlist_clear(void);

#ifdef __cplusplus
}
#endif


void csclearlist_add(void (CS_CALLBACK *pfunc_clear)(void* pargs), void* pargs)
{
    struct list_clear_t* node_clear = NULL;

    node_clear = (struct list_clear_t*)malloc(sizeof(struct list_clear_t));
    node_clear->clear_item.pfunc_clear = pfunc_clear;
    node_clear->clear_item.pargs = pargs;

    list_add(&node_clear->listnode, &s_list_clear);
}

void csclearlist_clear()
{
    s_execute_clears();
    s_clearlist_clear();
}

void s_execute_clears()
{
    struct list_clear_t* node_clear = NULL;
    struct list_head* execnode = s_list_clear.prev;

    while (execnode != (&s_list_clear)) {
        node_clear = container_of(execnode, struct list_clear_t, listnode);
        node_clear->clear_item.pfunc_clear(node_clear->clear_item.pargs);
        execnode = execnode->prev;
    }
}

void s_clearlist_clear()
{
    struct list_clear_t* node_clear = NULL;
    struct list_head* delnode = s_list_clear.next;

    while (delnode != (&s_list_clear)) {
        node_clear = container_of(delnode, struct list_clear_t, listnode);
        delnode = delnode->next;
        free(node_clear);
    }

    s_list_clear.prev = &s_list_clear;
    s_list_clear.next = &s_list_clear;
}
