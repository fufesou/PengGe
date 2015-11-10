/**
 * @file account_macros.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周二 2015-11-10 19:44:44 中国标准时间
 */

#ifndef _ACCOUNT_MACROS_H
#define _ACCOUNT_MACROS_H

#define __in
#define __out
#define __inout

#define SIZEOF_ARR(arr) (sizeof(arr)/sizeof((arr)[0]))

#define DECLARE_ACCOUNT_METHOD(method) \
    int am_##method##_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen); \
    int am_##method##_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen);

#define INVALID_METHOD_ID 0xFFFFFFFF
#define INVALID_USER_ID 0xFFFFFFFF

#define REGISTER_ACCOUNT_PROCESS_BEGIN \
    static struct account_method_t s_methodarr[] = { 

#define REGISTER_ACCOUNT_PROCESS(method)
        { \
            .methodname = #method, \
            .reply = am_##method##_reply, \
            .react = am_##method##_react \
        }, 

#define REGISTER_ACCOUNT_PROCESS_END \
    };


#endif //ACCOUNT_MACROS_H
