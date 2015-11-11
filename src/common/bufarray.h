/**
 * @file bufarray.h
 * @brief  The basic buffer operations that implemented with circular queue.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-17
 */

#ifndef  BUFARRAY_H
#define  BUFARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief array_buf is used to hold data. This struct works like circle queue(tail-in head-out).
 *
 * @note For function members in the struct, the pointer of current owner struct (commonly the first parameter)
 * can be omited, if i use 'container of' macro the in implementation. This will make the function called more intuitive.
 * However i have not seen this usage, thus i am not sure whether this usage is harmful to efficiency.
 */
struct array_buf {
    /**
     * @brief head index the first item that hold data.
     */
    int head;
    /**
     * @brief tail index the item after the last item with data.
     */
    int tail;
    /**
     * @brief len_item is the length of each buffer item.
     * This value is multiple of 512 right this moment.
     */
    int len_item;
    /**
     * @brief num_item is the number of items that the buffer hold.
     * Data number is multiple of 4 right this moment. The actual
     * num_item value is the number of data item plus one(tail
     * item with no data).
     */
    int num_item;
    /**
     * @brief data is 2D array.
     * data holds the indices of items.
     * *data holds the buffered data.
     */
    char** data;

    /**
     * @brief  pull_item fetch the earliest item in array_buf object.
     *
     * @param buf is the array_buf object that hold data.
     *
     * @return data to handle.
     */
    char* (*pull_item)(struct array_buf* buf);
    /**
     * @brief  push_item will push item in array_buf object.
     *
     * @param buf is the array_buf object that will accept item data.
     * @param item is the data to form new item of buf.
     *
     * @return two difference values.
     * NULL if push_item failed.
     * item if push_item succeed.
     */
    char* (*push_item)(struct array_buf* buf, char* item);
    /**
     * @brief clear_buf does some clear work for the array_buf object.
     *
     * @param buf is the struct array_buf object that will be clear.
     */
    void (*clear_buf)(struct array_buf* buf);

    int (*get_num_contained_item)(const struct array_buf* buf);
};


/**
 * @brief  init_buf initialize struct array_buf*.
 *
 * @param buf is the struct array_buf* to init.
 * @param numitem is the number of items in struct array_buf. 
 * 'num_item' will be set to the smallest number that fit: 
 * ((num_item >= numitem) && ((num_item % 4) == 0))
 * @param lenitem is the length of every item in struct array_buf.
 * 'len_item' will be set to the smallest number that fit:
 * ((len_item >= lenitem) && ((len_iem % 512) == 0))
 * @param nmalloc is the number of item data to be malloc. This parameter
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
