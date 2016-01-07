/**
 * @file bufarray.c
 * @brief  0.1
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-17
 * @modified  Mon 2015-12-07 20:23:30 (+0800)
 */

#include  <malloc.h>
#include  <string.h>
#include  <stdio.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/bufarray.h"

void init_buf(struct array_buf* buf, int numitem, int lenitem, int nmalloc)
{
    int i = 0;

    buf->head = 0;
    buf->len_item = TO_MULTIPLE_OF(lenitem, 512);
    buf->num_item = TO_MULTIPLE_OF(numitem, 4) + 1;

    buf->data = (char**)calloc(buf->num_item, sizeof(char*));
    for (i=0; i<buf->num_item; ++i) {
        buf->data[i] = NULL;
    }

    if (nmalloc < 0 || nmalloc >= numitem) {
        nmalloc = buf->num_item - 1;
    }
    else {
        nmalloc = TO_MULTIPLE_OF(nmalloc, 4);
    }

    buf->tail = nmalloc;

    for (i=0; i<nmalloc; ++i) {
        buf->data[i] = (char*)calloc(buf->len_item, sizeof(char));
        memset(buf->data[i], 0, buf->len_item);
    }
}

char* pull_item(struct array_buf* buf)
{
    char* item = NULL;
#ifdef _CHECK_ARGS
    if (buf == NULL) {
        return NULL;
    }
#endif

    if (buf->head == buf->tail) {
        return NULL;
    }
    item = buf->data[buf->head];
    buf->head = ((buf->head + 1) == buf->num_item) ?
                    0 : (buf->head + 1);
    return item;
}

char* push_item(struct array_buf* buf, char* item)
{
    int ahead_tail = ((buf->tail + 1) == buf->num_item) ?
                            0 : (buf->tail + 1);

#ifdef _CHECK_ARGS
    if (item == NULL || buf == NULL) {
        return NULL;
    }
#endif

    if (ahead_tail == buf->head) {
        return NULL;
    }

    buf->data[buf->tail] = item;
    buf->tail = ahead_tail;

    return item;
}

void clear_buf(struct array_buf* buf)
{
    int itrhead = buf->head;
    int itrtail = buf->tail;
    while (itrhead != itrtail)
    {
        free(buf->data[itrhead]);
        itrhead = (itrhead + 1) % buf->num_item;
    }
    free(buf->data);
}

int get_num_contained_items(const struct array_buf* buf)
{
    int num = buf->tail + buf->num_item - buf->head;
    return (num > buf->num_item) ? (num - buf->num_item) : num;
}
