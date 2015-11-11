/**
 * @file client_account.c
 * @brief  This file defines some basic account process functions for client.
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周三 2015-11-11 19:10:34 中国标准时间
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
 * @brief  am_account_create_react This is the first message returned from server. And this message is just a notification message.
 *
 * @param inmsg This message tells the account creation result.
 * @param inmsglen
 * @param outmsg 
 * @param outmsglen
 *
 * @return   The return value has no meaning, and it always is 0.
 */
int am_account_create_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    (void)inmsglen;
    (void)outmsg;

    fprintf(stdout, "%s.\n", inmsg + sizeof(uint32_t)*2);
    *outmsglen = 0;

    return 0;
}


/**
 * @brief  am_account_login_react This function estimate whether the login succeed. The account information will be filled into outmsg if successfully logined in.
 *
 * @param inmsg This message contains the account login result and account information if succeed.
 * @param inmsglen
 * @param outmsg This pointer points to an account object.
 * @param outmsglen
 *
 * @return   0 if an account is created, 1 otherwise.
 */
int am_account_login_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    int ret = 0;
    if (strncmp(inmsg + sizeof(uint32_t)*2, g_succeed, strlen(g_succeed)) == 0) {
        ret = (cs_memcpy(
                    outmsglen, *outmsglen,
                    inmsg + sizeof(uint32_t)*2 + strlen(g_succeed), inmsglen - sizeof(uint32_t)*2 + strlen(g_succeed)));
    }
    *outmsglen = 0;
    return ret;
}

int am_account_inquire_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, inmsglen, outmsg, outmsglen);
}

int am_account_changeusername_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, inmsglen, outmsg, outmsglen);
}

int am_account_changepasswd_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, inmsglen, outmsg, outmsglen);
}

int am_account_changegrade_react(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, inmsglen, outmsg, outmsglen);
}

