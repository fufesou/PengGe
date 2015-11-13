/**
 * @file client_account.c
 * @brief  This file defines some basic account process functions for client.
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周三 2015-11-11 23:31:06 中国标准时间
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <sys/socket.h>
#endif

#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include    "account_macros.h"
#include    "utility_wrap.h"
#include    "sock_types.h"
#include    "msgwrap.h"
#include    "account.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char g_succeed;
extern char g_fail;

int s_genera_msg_create(const char* inmsg, uint32_t userid, uint32_t methodid, char* outmsg, __inout int* outmsglen);

#ifdef __cplusplus
}
#endif

int s_general_msg_create(const char* inmsg, uint32_t userid, uint32_t methodid, char* outmsg, __inout int* outmsglen)
{
    static int fixedlen = sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t);
    if (snprintf(outmsg, *outmsglen, "%d%d", userid, methodid) < 0) {
        return 1;
    }
    if (cs_memcpy(outmsg + fixedlen, (*outmsglen) - fixedlen, inmsg, GET_HEADER_DATA(inmsg, numbytes)) == 0) {
        *outmsglen = fixedlen + GET_HEADER_DATA(inmsg, numbytes);
        return 0;
    }
    return 1;
}

/**************************************************
 **             the request block                 **
 **************************************************/
#define REQUEST_GENERATE(methodname) \
    int am_##methodname##_request(const char* inmsg, uint32_t userid, char* outmsg, __inout int* outmsglen) \
    { \
        return s_general_msg_create(inmsg, userid, am_method_getid(#methodname), outmsg, outmsglen); \
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
int am_account_create_react(char* inmsg, char* outmsg, __inout int* outmsglen)
{
    static int fixedlen = sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t);
    (void)outmsg;

    if (inmsg[fixedlen] == g_succeed) {
        printf("client: create account succeed, please wait for a moment to receiving the random telcode, and revify it.\n");
    } else {
        printf("client: create account fail.\n");
    }

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
int am_account_login_react(char* inmsg, char* outmsg, __inout int* outmsglen)
{
    static int fixedlen = sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t);
    int ret = 0;

    (void)outmsg;

    if (inmsg[fixedlen] == g_succeed) {
        ret = (cs_memcpy(outmsglen, *outmsglen, inmsg + fixedlen + 1, sizeof(struct account_basic_t)));
    }
    if (inmsg[fixedlen + 1 + sizeof(struct account_basic_t)] != 0) {
        printf("client: additional message from server - %s.\n", inmsg + fixedlen + 1 + sizeof(struct account_basic_t));
    }
    *outmsglen = 0;
    return ret;
}

int am_account_inquire_react(char* inmsg, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, outmsg, outmsglen);
}

int am_account_changeusername_react(char* inmsg, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, outmsg, outmsglen);
}

int am_account_changepasswd_react(char* inmsg, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, outmsg, outmsglen);
}

int am_account_changegrade_react(char* inmsg, char* outmsg, __inout int* outmsglen)
{
    return am_account_login_react(inmsg, outmsg, outmsglen);
}

