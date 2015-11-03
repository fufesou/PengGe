/**
 * @file msgdispatch.h
 * @brief This file provide differenct kinds of message handler functions.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 */

#ifndef  MSGDISPATCH_H
#define  MSGDISPATCH_H

#ifdef __cplusplus
extern "C" {
#endif

void process_msg(char* inmsg, char* outmsg, int len_outmsg);


#ifdef __cplusplus
}
#endif

#endif  // MSGDISPATCH_H
