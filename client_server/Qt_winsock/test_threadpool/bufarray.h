/**
 * @file bufarray.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-17
 */

#ifndef  BUFARRAY_H
#define  BUFARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

struct array_buf {
    int head;
    int tail;
    int len_item;
    int num_item;
    char** data;

    char* (*pull_item)(struct array_buf*);
    char* (*push_item)(struct array_buf*, char*);
    void (*clear_buf)(struct array_buf*);
};


/**
 * @brief  init_buf set the initial data and operations for struct array_buf*.
 *
 * @param buf is the struct array_buf* to init.
 * @param numitem is the number of items in struct array_buf. 
 * 'num_item' will be set to the smallest number that fit: 
 * ((num_item >= numitem) && ((num_item % 4) == 0))
 * @param lenitem is the length of every item in struct array_buf.
 * 'len_item' will be set to the smallest number that fit:
 * ((len_item >= lenitem) && ((len_iem % 512) == 0))
 * #param nmalloc is the number of item data to be malloc. This parameter
 * will be change in the following principles in the function:
 * 1. if nmalloc is less than 0 or greater equal than 'numitem', nmalloc will
 *      be set to 'numitem'.
 * 2. otherwise nmalloc will be set to the smallest number that fit:
 *      ((nmalloc >= numitem) && ((nmalloc % 4) == 0))
 */
void init_buf(struct array_buf* buf, int numitem, int lenitem, int nmalloc);


#ifdef __cplusplus
}
#endif

#endif  // BUFARRAY_H
