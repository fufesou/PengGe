/**
 * @file buflist.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-16
 */

#include  <malloc.h>
#include  <stdio.h>
#include  <assert.h>
#include  "list.h"
#include  "buflist.h"
#include  "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

static struct buf_node* s_create_bufnode(int buflen);


#ifdef __cplusplus
}
#endif

struct buf_node* s_create_bufnode(int buflen)
{
    struct buf_node* pnode = (struct buf_node*)malloc(sizeof(struct buf_node));
    pnode->buf = (char*)malloc(buflen);
    pnode->buflen = buflen;
    return pnode;
}

void init_buflist(struct list_head* listhead, int listlen, int buflen)
{
    int i = 0;
    struct buf_node* newnode = NULL;
    buflen = TO_MULTIPLE_OF(buflen, 512);

    while (i < listlen) {
        newnode = s_create_bufnode(buflen);
        list_add(&newnode->listnode, listhead);
        ++i;
    }
}

void destroy_buflist(struct list_head* listhead)
{
    struct list_head* delnode = listhead->next;
    struct buf_node* del_bufnode = NULL;

    while (delnode != listhead) {
        del_bufnode = container_of(delnode, struct buf_node, listnode);
        free(del_bufnode->buf);
        free(del_bufnode);
        delnode = delnode->next;
    }

    listhead->prev = listhead;
    listhead->next = listhead;
}

struct buf_node* pullbuf(struct list_head* listhead)
{
    struct list_head* pnode = NULL;
    if (list_empty(listhead)) {
        return NULL;
    }
    pnode = listhead->prev;
    assert(pnode->next != 0);
    list_del(pnode);
    return container_of(pnode, struct buf_node, listnode);
}

void pushbuf(struct buf_node* node, struct list_head* listhead)
{
    if (node == 0) {
        return;
    }
    list_add(&node->listnode, listhead);
}

