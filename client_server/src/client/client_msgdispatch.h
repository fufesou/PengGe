/**
 * @file client_msgdispatch.h
 * @brief  
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

void csclient_process_msg(char* inmsg, char* outmsg, int* outmsglen, void* clidata);

#ifdef __cplusplus
}
#endif

#endif //CLIENT_MSGDISPATH_H
