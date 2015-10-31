/**
 * @file server_servroutine.h
 * @brief This file provide the main loop communication work process for server.
 * With buffer and thread pool working, server can be more powerful in concurrent process.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 */

#ifndef  SERVER_SERVROUTINE_H
#define  SERVER_SERVROUTINE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief process_communication process_communication is an infinite loop process for server.
 *
 * @param serv_udp serv_udp contains the necessary information for sending and receiving message.
 */
void process_communication(struct server_udp* serv_udp);


#ifdef __cplusplus
}
#endif

#endif  // SERVER_SERVROUTINE_H
