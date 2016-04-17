/**
 * @file msgwrap.h  
 * @brief  This file describe the message header for sending and receiving.
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-19
 * @modified  Mon 2016-01-04 23:58:31 (+0800)
 */

#ifndef  MSGUNIT_H
#define  MSGUNIT_H

#ifdef __cplusplus
extern "C" {
#endif

#define GET_HEADER_DATA(ptr, mem) (((struct jxmsg_header*)ptr)->mem)

#pragma pack(4)
/**
 * @brief  jxmsg_header describe the header in pool item. jxmsg_header is followed by the actual message data.
 *
 * @todo This message header shoud be devided into two parts: 1. used in this host 2. transmit between client and server.
 * 'addr' and 'addrlen' belong to the first part, 'mlfag', 'header', and 'numbytes' belong to the second part.
 */
struct jxmsg_header {
    /**
     * @brief addr addr is only used in current host. It will be updated inmmediately after receiving.
     */
    struct sockaddr addr;

    /**
     * @brief addrlen addrlen is only used in current host. It will be updated inmmediately after receiving.
     */
    uint8_t addrlen;

    /**
     * @brief mflag the message flag.
     *
     * @sa jxiot.h
     */
    mflag_t mflag;

    /**
     * @brief header 'header' will be transmit between client and server.
     * But as the server does not change this data, it is not necessary to change the data between host and net endian.
     */
    struct hdr header;

    /**
     * @brief numbytes numbytes will be transmit between client and server. Thus the conversion between host and net should be apply to it.
     */
    uint32_t numbytes;
};
#pragma pack()

/**
 * @brief  jxmsg_copyaddr This function set the value of address information.
 *
 * @param msghdr
 * @param addr
 * @param addrlen
 *
 * @return   0 if succeed, 1 if fail.
 */
int jxmsg_copyaddr(struct jxmsg_header* msghdr, const struct sockaddr* addr, uint8_t addrlen);

/**
 * @brief  jxmsg_merge 
 *
 * @param msgheader
 * @param data
 * @param unit
 * @param unitlen
 *
 * @return   0 if succeed, 1 if fail.
 */
int jxmsg_merge(const struct jxmsg_header* msghdr, const char* data, char* unit, uint32_t unitlen);

/**
 * @brief  jxmsg_extract 
 *
 * @param unit
 * @param msgheader
 * @param data
 */
void jxmsg_extract(const char* unit, const struct jxmsg_header** msgheader, const char** data);

/**
 * @brief  jxmsg_extract_copy 
 *
 * @param unit
 * @param msgheader
 * @param data
 * @param datalen
 *
 * @return   
 */
int jxmsg_extract_copy(const char* unit, struct jxmsg_header* msgheader, char* data, int datalen);


#ifdef __cplusplus
}
#endif


#endif  // MSGUNIT_H
