/**
 * @file list.h
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-15
 */

#ifndef  CLIST_H
#define  CLIST_H

#ifdef __cplusplus
extern "C" {
#endif


struct lnode
{
    struct lnode* pPreNode;
    struct lnode* pNextNode;
};

#define LIST_INIT(name) { &(name), &(name) }
#define LIST(name) struct lnode name = LIST_INIT(name)

typedef struct lnode LNode;
typedef struct lnode* List;

static inline void init_list(struct lnode* vList)
{
    vList->pPreNode = vList;
    vList->pNextNode = vList;
}

List createList();
void destroyList();
int insert(LNode* vPos, LNode* vNewNode);
int preinsert(LNode* vPos, LNode* vNewNode);
LNode* delNode(LNode* vNode);


#ifdef __cplusplus
}
#endif

#endif  // CLIST_H
