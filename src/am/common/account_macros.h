/**
 * @file account_macros.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  ���� 2015-11-11 23:33:54 �й���׼ʱ��
 */

#ifndef _ACCOUNT_MACROS_H
#define _ACCOUNT_MACROS_H

#ifndef __in
#define __in
#endif

#ifndef __out
#define __out
#endif

#ifndef __inout
#define __inout
#endif

#define SIZEOF_ARR(arr) (sizeof(arr)/sizeof((arr)[0]))

#define DECLARE_ACCOUNT_METHOD(method) \
    int am_##method##_reply(char* inmsg, char* outmsg, __inout int* outmsglen); \
    int am_##method##_react(char* inmsg, char* outmsg, __inout int* outmsglen);

#define INVALID_METHOD_ID 0xFFFFFFFF
#define INVALID_USER_ID 0xFFFFFFFF



#endif //ACCOUNT_MACROS_H
