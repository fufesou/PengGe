/**
 * @file account_macros.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Mon 2015-11-16 18:57:05 (+0800)
 */

#ifndef _ACCOUNT_MACROS_H
#define _ACCOUNT_MACROS_H

#ifndef __jxin
#define __jxin
#endif

#ifndef __jxout
#define __jxout
#endif

#ifndef __jxinout
#define __jxinout
#endif


#define DECLARE_ACCOUNT_METHOD(method) \
    int am_##method##_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen); \
    int am_##method##_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen);

#define INVALID_METHOD_ID 0xFFFFFFFF
#define INVALID_USER_ID 0xFFFFFFFF



#endif //ACCOUNT_MACROS_H
