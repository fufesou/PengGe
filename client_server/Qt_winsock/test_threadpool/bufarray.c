/**
 * @file bufarray.c
 * @brief  0.1
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-17
 */

#include  <malloc.h>
#include  <string.h>
#include  <stdio.h>
#include    "bufarray.h"
#include    "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

static void s_init_buf(struct array_buf* buf, int numitem, int lenitem, int nmalloc);
static char* s_pull_item(struct array_buf* buf);
static char* s_push_item(struct array_buf* buf, char* item);
static void s_clear_buf(struct array_buf* buf);

void init_buf(struct array_buf* buf, int numitem, int lenitem, int nmalloc)
{
    s_init_buf(buf, numitem, lenitem, nmalloc);
}

void s_init_buf(struct array_buf* buf, int numitem, int lenitem, int nmalloc)
{
    int i = 0;

    buf->head = 0;
    buf->len_item = TO_MULTIPLE_OF(lenitem, 512);
    buf->num_item = TO_MULTIPLE_OF(numitem, 4);
    buf->pull_item = s_pull_item;
    buf->push_item = s_push_item;
    buf->clear_buf = s_clear_buf;

    buf->data = (char**)malloc(sizeof(char*) * buf->num_item);
    for (i=0; i<buf->num_item; ++i) {
        buf->data[i] = NULL;
    }

    if (nmalloc < 0 || nmalloc >= numitem) {
        nmalloc = buf->num_item;
    }
    nmalloc = TO_MULTIPLE_OF(nmalloc, 4);

    buf->tail = nmalloc;

    for (i=0; i<nmalloc; ++i) {
        buf->data[i] = (char*)malloc(sizeof(char) * buf->len_item);
        memset(buf->data[i], 0, buf->len_item);
    }
}

char* s_pull_item(struct array_buf* buf)
{
    char* item = NULL;
#ifdef CHECK_PARAM
    if (buf == NULL) {
        return NULL;
    }
#endif

    if (buf->head == buf->tail) {
        return NULL;
    }
    item = buf->data[buf->head];
    buf->head = (buf->head + 1) % buf->num_item;
    return item;
}

char* s_push_item(struct array_buf* buf, char* item)
{
    int ahead_tail = (buf->tail + 1) % buf->num_item;
#ifdef CHECK_PARAM
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

void s_clear_buf(struct array_buf* buf)
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

#ifdef __cplusplus
}
#endif
