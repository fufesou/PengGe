/**
 * @file client_account.c
 * @brief  This file defines some basic account process functions for client.
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周二 2015-11-10 20:07:37 中国标准时间
 */

#include    "account_macros.h"
#include    "utility_wrap.h"
#include    "account.h"

#ifdef __cplusplus
extern "C" {
#endif

char* g_succeed;
char* g_fail;

int s_genera_msg_create(const char* inmsg, int inmsglen, uint32_t userid, uint32_t methodid, char* outmsg, __inout int* outmsglen);

#ifdef __cplusplus
}
#endif

int s_general_msg_create(const char* inmsg, int inmsglen, uint32_t userid, uint32_t methodid, char* outmsg, __inout int* outmsglen)
{
    if (snprintf(outmsg, *outmsglen, "%d%d", userid, methodid) < 0) {
        return 1;
    }
    if (cs_memcpy(outmsg + sizeof(userid) + sizeof(methodid), (*outmsglen) - sizeof(userid) - sizeof(methodid), inmsg, inmsglen) == 0) {
        *outmsglen = sizeof(userid) + sizeof(methodid) + inmsglen;
        return 0;
    }
    return 1;
}

/**************************************************
 **             the request block                 **
 **************************************************/
#define REQUEST_GENERATE(methodname) \
    int am_##methodname##_request(const char* inmsg, int inmsglen, uint32_t userid, char* outmsg, __inout int* outmsglen) \
    { \
        return s_general_msg_create(inmsg, inmsglen, userid, am_method_getid(#methodname), outmsg, outmsglen); \
    }

REQUEST_GENERATE(account_create)
REQUEST_GENERATE(account_create_verify)
REQUEST_GENERATE(account_login)
REQUEST_GENERATE(account_inquire)
REQUEST_GENERATE(account_changeusername)
REQUEST_GENERATE(account_changepasswd)
REQUEST_GENERATE(account_changegrade)

#undef REQUEST_GENERATE


/**************************************************
 **             the reply block                 **
 **************************************************/
/**
 * @brief  am_account_create_react The inmsg is the result of creating account.
 *
 * @param inmsg This pointer actual points to the account.
 * @param inmsglen
 * @param outmsg 
 * @param outmsglen
 *
 * @return   
 */
int am_account_create_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    int ret = 0;
    if (strncmp(inmsg + sizeof(uint32_t)*2, g_succeed, strlen(g_succeed)) == 0) {
        ret = (cs_memcpy(
                    outmsglen, *outmsglen,
                    inmsg + sizeof(uint32_t)*2 + strlen(g_succeed), inmsglen - sizeof(uint32_t)*2 + strlen(g_succeed)));
    }
    *outmsglen = 0;
    return 1;
}

/**
 * @brief  am_account_create_verify_react 
 *
 * @param inmsg
 * @param inmsglen
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_create_verify_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_login_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_inquire_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_changeusername_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_changepasswd_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_changegrade_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

