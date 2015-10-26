/**
 * @file msgwrap.h	
 * @brief  This file describe the message for sending and receiving.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Mon 2015-10-26 18:29:53 (+0800)
 */

#ifndef  MSGUNIT_H
#define  MSGUNIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  msg_header describe the header in pool item. msg_header is followed by the actual message data.
 */
struct msg_header {
    SOCKADDR_IN addr;
    int addrlen;
    struct hdr header;
    ssize_t numbytes;
};

/**
 * @brief  merge2unit 
 *
 * @param hdr_unit
 * @param data
 * @param unit
 * @param unitlen
 *
 * @return   
 */
int merge2unit(const struct msg_header* hdr_unit, const char* data, char* unit, int unitlen);

/**
 * @brief  extract_msg 
 *
 * @param unit
 * @param hdr_unit
 * @param data
 */
void extract_msg(const char* unit, const struct msg_header** hdr_unit, const char** data);

/**
 * @brief  extract_copy_msg 
 *
 * @param unit
 * @param hdr_unit
 * @param data
 * @param datalen
 *
 * @return   
 */
int extract_copy_msg(const char* unit, struct msg_header* hdr_unit, char* data, int datalen);

/**
 * @brief  push2pool will firstly copy message data and the unit header to single char array. And 
 * then push the single char array to struct sendrecv_pool. The procedure can be splitted into 
 * three steps:
 * 1. take out one item from empty buffer in the pool.
 * 2. copy data and unithdr to the item.
 * 3. push the item into the filled buffer
 *
 * @param data the message data.
 * @param unithdr the unit header struct.
 * @param pool the pool to operate.
 *
 * @return  
 * 0 if success. There is en empty buffer for operation.
 * 1 if fail. There is no empty buffer left.
 */
int push2pool(const char* data, const struct msg_header* unithdr, struct sendrecv_pool* pool);

/**
 * @brief  pull_from_pool works the opposite as push2pool does. The procedure can be splitted into
 * three steps:
 * 1. take out one item from filled buffer in pool.
 * 2. copy data and unithdr from the item.
 * 3. push the item into the empty buffer.
 *
 * @param data the message data
 * @param datalen the length of message data.
 * @param unithdr the unit header struct.
 * @param pool the pool to operate.
 *
 * @return   
 * 0 if success. There is a filled buffer for operation.
 * 1 if fail. There is no filled buffer left.
 *
 * @see push2pool
 */
int pull_from_pool(char* data, int datalen, struct msg_header* unithdr, struct sendrecv_pool* pool);

#ifdef __cplusplus
}
#endif


#endif  // MSGUNIT_H
