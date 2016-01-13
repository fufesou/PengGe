/**
 * @file regportlist.h
 * @brief  This file defines the interfaces of register port process, such as register, query, delete, etc.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2016-01-12
 * @modified  Wed 2016-01-13 22:54:03 (+0800)
 */

#ifndef _REGPORTLIST_H
#define _REGPORTLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  jxregportlist_register this function will conbine ip address and port with in one structure object
 * as a node of register port list.
 *
 * if the ip address exists before, the port will be changed based on the force parameter.
 *
 * @param ip        the ip address.
 * @param iplen     the length of ip address.
 * @param port      the port is in network order.
 * @param force     if not set, existing ip&port will not be replaced, otherwise, port will be updated.
 *
 * @return   0 if ip address has not been registered, 1 otherwise.
 */
int jxregportlist_register(void* ip, uint8_t iplen, unsigned short port, int force);

/**
 * @brief  jxregportlist_query this function will query for the port based on the ip address.
 *
 * @param ip
 * @param port      the port is in network order.
 *
 * @return   0 if ip address has been registered and port is set, 1 otherwise.
 */
int jxregportlist_query(void* ip, unsigned short* port);

/**
 * @brief  jxregportlist_del 
 *
 * @param ip
 *
 * @return   0 if ip address has been registered, 1 otherwise.
 */
int jxregportlist_del(void* ip);

#ifdef __cplusplus
}
#endif

#endif //REGPORTLIST_H
