/**
 * @file processlist.c
 * @brief  this file contains the utility implementations of struct list_process_t.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2016-01-05
 * @modified  Tue 2016-01-05 23:08:55 (+0800)
 */

#include  <malloc.h>

#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/list.h"
#include    "common/error.h"
#include    "common/processlist.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif


void jxprocesslist_set(struct list_head* processlist_head, uint8_t mflag, pfunc_msgprocess_t pfunc_process, pfunc_msgprocess_af_t pfunc_process_af, int force)
{
    struct list_head* pnode = processlist_head->next;
    struct list_process_t* pnode_process = NULL;

    if ((pfunc_process == NULL) && (pfunc_process_af == NULL)) {
        jxwarning("parameter is not valid, nothing changed.\n");
    }

    while (pnode != processlist_head) {
        pnode_process = container_of(pnode, struct list_process_t, listnode);
        if (mflag == pnode_process->mflag) {
            if (pfunc_process != NULL) {
                pnode_process->pprocess = pfunc_process;
            } else if (force) {
                pnode_process->pprocess = NULL;
            }
            if (pfunc_process_af != NULL) {
                pnode_process->pprocess_af = pfunc_process_af;
            } else if (force) {
                pnode_process->pprocess_af = NULL;
            }

            return ;
        } 
        pnode = pnode->next;
    }

    pnode_process = (struct list_process_t*)calloc(sizeof(struct list_process_t), 1);
    pnode_process->mflag = mflag;
    pnode_process->pprocess = pfunc_process;
    pnode_process->pprocess_af = pfunc_process_af;

    list_add(&pnode_process->listnode, processlist_head);
}

void jxprocesslist_clear(void* processlist_head)
{
    struct list_head* phead = (struct list_head*)processlist_head;
    struct list_head* pnode = phead->next;
    struct list_process_t* pnode_process = NULL;

    while (pnode != phead) {
        pnode_process = container_of(pnode, struct list_process_t, listnode);
        pnode = pnode->next;
        list_del(&pnode_process->listnode);
        free(pnode_process);
    }
}

int jxprocesslist_process_valid(const struct list_head* processlist_head)
{
    struct list_head* pnode = processlist_head->next;
    struct list_process_t* pnode_process = NULL;

    while (pnode != processlist_head) {
        pnode_process = container_of(pnode, struct list_process_t, listnode);
        if (pnode_process->pprocess != NULL) {
            return 1;
        } 
        pnode = pnode->next;
    }

    return 0;
}

int jxprocesslist_process_af_valid(const struct list_head* processlist_head)
{
    struct list_head* pnode = processlist_head->next;
    struct list_process_t* pnode_process = NULL;

    while (pnode != processlist_head) {
        pnode_process = container_of(pnode, struct list_process_t, listnode);
        if (pnode_process->pprocess_af != NULL) {
            return 1;
        } 
        pnode = pnode->next;
    }

    return 0;
}

pfunc_msgprocess_t jxprocesslist_process_get(const struct list_head* processlist_head, uint8_t mflag)
{
    struct list_head* pnode = processlist_head->next;
    struct list_process_t* pnode_process = NULL;

    while (pnode != processlist_head) {
        pnode_process = container_of(pnode, struct list_process_t, listnode);
        if (mflag == pnode_process->mflag) {
            return pnode_process->pprocess;
        } 
        pnode = pnode->next;
    }

    return NULL;
}

pfunc_msgprocess_af_t jxprocesslist_process_af_get(const struct list_head* processlist_head, uint8_t mflag)
{
    struct list_head* pnode = processlist_head->next;
    struct list_process_t* pnode_process = NULL;

    while (pnode != processlist_head) {
        pnode_process = container_of(pnode, struct list_process_t, listnode);
        if (mflag == pnode_process->mflag) {
            return pnode_process->pprocess_af;
        } 
        pnode = pnode->next;
    }

    return NULL;
}

