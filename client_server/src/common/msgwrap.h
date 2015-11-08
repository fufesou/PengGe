/**
 * @file msgwrap.h	
 * @brief  This file describe the message header for sending and receiving.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  周五 2015-11-06 12:02:41 中国标准时间
 */

#ifndef  MSGUNIT_H
#define  MSGUNIT_H

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(4)
/**
 * @brief  csmsg_header describe the header in pool item. csmsg_header is followed by the actual message data.
 */
struct csmsg_header {
    struct sockaddr addr;
    int addrlen;
    struct hdr header;
    int numbytes;
};
#pragma pack()

/**
 * @brief  csmsg_copyaddr This function set the value of address information.
 *
 * @param msghdr
 * @param addr
 * @param addrlen
 *
 * @return   0 if succeed, 1 if fail.
 */
int csmsg_copyaddr(struct csmsg_header* msghdr, const struct sockaddr* addr, int addrlen);

/**
 * @brief  csmsg_merge 
 *
 * @param msgheader
 * @param data
 * @param unit
 * @param unitlen
 *
 * @return   0 if succeed, 1 if fail.
 */
int csmsg_merge(const struct csmsg_header* msghdr, const char* data, char* unit, int unitlen);

/**
 * @brief  csmsg_extract 
 *
 * @param unit
 * @param msgheader
 * @param data
 */
void csmsg_extract(const char* unit, const struct csmsg_header** msgheader, const char** data);

/**
 * @brief  csmsg_extract_copy 
 *
 * @param unit
 * @param msgheader
 * @param data
 * @param datalen
 *
 * @return   
 */
int csmsg_extract_copy(const char* unit, struct csmsg_header* msgheader, char* data, int datalen);

/**
 * @brief  csmsg_push2pool will firstly copy message data and the unit header to single char array. And 
 * then push the single char array to struct csmsgpool. The procedure can be splitted into 
 * three steps:
 * 1. take out one item from empty buffer in the pool.
 * 2. copy data and msgheader to the item.
 * 3. push the item into the filled buffer
 *
 * @param data the message data.
 * @param msgheader the unit header struct.
 * @param pool the pool to operate.
 *
 * @return  
 * 0 if success. There is en empty buffer for operation.
 * 1 if fail. There is no empty buffer left.
 */
int csmsg_push2pool(const char* data, const struct csmsg_header* msgheader, struct csmsgpool* pool);

/**
 * @brief  csmsg_pull_from_pool works the opposite as csmsg_push2pool does. The procedure can be splitted into
 * three steps:
 * 1. take out one item from filled buffer in pool.
 * 2. copy data and msgheader from the item.
 * 3. push the item into the empty buffer.
 *
 * @param data the message data
 * @param datalen the length of message data.
 * @param msgheader the unit header struct.
 * @param pool the pool to operate.
 *
 * @return   
 * 0 if success. There is a filled buffer for operation.
 * 1 if fail. There is no filled buffer left.
 *
 * @see csmsg_push2pool
 */
int csmsg_pull_from_pool(char* data, int datalen, struct csmsg_header* msgheader, struct csmsgpool* pool);

#ifdef __cplusplus
}
#endif


#endif  // MSGUNIT_H
