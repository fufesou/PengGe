/**
 * @file client_msgdispatch.h
 * @brief  This file provides the process functions for message received from server. And the process function dispatches the message to corresponding handler.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-07
 * @modified  Sat 2015-11-07 14:47:04 (+0800)
 */

#ifndef _CLIENT_MSGDISPATH_H
#define _CLIENT_MSGDISPATH_H

#ifdef __cplusplus
extern "C"
{
#endif

void csclient_process_msg(char* inmsg, char* outmsg, uint32_t* outmsglen, void* clidata);

#ifdef __cplusplus
}
#endif

#endif //CLIENT_MSGDISPATH_H
