/**
 * @file regportlist.c
 * @brief  this file will define the register port list structure and the implementation of register port list operations.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2016-01-12
 * @modified  Wed 2016-01-13 22:53:27 (+0800)
 */

#include  <malloc.h>
#include  <string.h>
#include  <stdio.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/utility_wrap.h"
#include    "common/list.h"
#include    "common/clearlist.h"
#include    "common/regportlist.h"

struct list_regport_t
{
    struct list_head    listnode;
    char*               ipaddr;
    uint8_t             ipaddrlen;
    unsigned short      port;       /**< port is in network order. */
};

static int s_inited = 0;
static LIST_HEAD(s_list_regport);

#ifdef __cplusplus
extern "C"
{
#endif

static void s_regportlist_init(void);
static void JXIOT_CALLBACK s_regportlist_clear(void* unused);
static struct list_regport_t* s_findnode(void* ipaddr);
static void s_addnode(void* ipaddr, uint8_t ipaddrlen, unsigned short port);

#ifdef __cplusplus
}
#endif

int jxregportlist_register(void* ip, uint8_t iplen, unsigned short port, int force)
{
    struct list_regport_t* pnode_regport = NULL;

    if (!s_inited) {
        s_regportlist_init();
        s_inited = 1;
    }

    if((pnode_regport = s_findnode(ip)) != NULL) {
        if (force) {
            pnode_regport->port = port;
        }
        return 1;
    }

    s_addnode(ip, iplen, port);
    return 0;
}

int jxregportlist_query(void* ip, unsigned short* port)
{
    struct list_regport_t* pnode_regport = NULL;
    if ((pnode_regport = s_findnode(ip)) != NULL) {
        *port = pnode_regport->port;
        return 0;
    }
    return 1;
}

int jxregportlist_del(void* ip)
{
    struct list_regport_t* pnode_regport = NULL;
    if ((pnode_regport = s_findnode(ip)) != NULL) {
        list_del(&pnode_regport->listnode);
        free(pnode_regport->ipaddr);
        free(pnode_regport);
        return 0;
    }
    return 1;
}

void s_regportlist_init(void)
{
    jxclearlist_add(s_regportlist_clear, NULL);
}

void JXIOT_CALLBACK s_regportlist_clear(void* unused)
{
    struct list_head* pnode = s_list_regport.next;
    struct list_regport_t* pnode_regport = NULL;

    while (pnode != &s_list_regport) {
        pnode_regport = container_of(pnode, struct list_regport_t, listnode);
        pnode = pnode->next;
        list_del(&pnode_regport->listnode);
        free(pnode_regport->ipaddr);
        free(pnode_regport);
    }

    (void)unused;
}

struct list_regport_t* s_findnode(void* ipaddr)
{
    struct list_head* pnode = s_list_regport.next;
    struct list_regport_t* pnode_regport = NULL;

    while (pnode != &s_list_regport) {
        pnode_regport = container_of(pnode, struct list_regport_t, listnode);
        if (memcmp(pnode_regport->ipaddr, ipaddr, pnode_regport->ipaddrlen) == 0) {
            return pnode_regport;
        }
        pnode = pnode->next;
    }

    return NULL;
}

void s_addnode(void* ipaddr, uint8_t ipaddrlen, unsigned short port)
{
    struct list_regport_t* pnode_regport = (struct list_regport_t*)malloc(sizeof(struct list_regport_t));
    pnode_regport->ipaddr = malloc(ipaddrlen);
    pnode_regport->ipaddrlen = ipaddrlen;
    jxmemcpy(pnode_regport->ipaddr, pnode_regport->ipaddrlen, ipaddr, ipaddrlen);
    pnode_regport->port = port;
    list_add(&pnode_regport->listnode, &s_list_regport);
}
