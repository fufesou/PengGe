/**
 * @file msgdispatch.h
 * @brief This file provide differenct kinds of message handler functions.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 * @modified  Wed 2015-11-04 18:52:43 (+0800)
 */

#ifndef  MSGDISPATCH_H
#define  MSGDISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif

void cserver_sprocess_msg(char* inmsg, char* outmsg, int* outmsglen);


#ifdef __cplusplus
}
#endif

#endif  // MSGDISPATCH_H
