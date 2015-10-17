/**
 * @file buflist.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-16
 */

#ifndef  BUFLIST_H
#define  BUFLIST_H

#ifdef __cplusplus
extern "C" {
#endif

struct buf_node
{
    struct list_head listnode;
    char* buf;
    int buflen;
};

void init_buflist(struct list_head* listhead, int listlen, int buflen);
void destroy_buflist(struct list_head* listhead);
struct buf_node* pullbuf(struct list_head* listhead);
void pushbuf(struct buf_node* node, struct list_head* listhead);

#ifdef __cplusplus
}
#endif


#endif  // BUFLIST_H
