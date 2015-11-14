/**
 * @file server_msgdispatch.h
 * @brief This file provide differenct kinds of message process functions, and dispatches message to corresponding handler.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 * @modified  Sat 2015-11-07 13:22:21 (+0800)
 */

#ifndef  MSGDISPATCH_H
#define  MSGDISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif

void csserver_process_msg(char* inmsg, char* outmsg, uint32_t* outmsglen, void* unused);


#ifdef __cplusplus
}
#endif

#endif  // MSGDISPATCH_H
