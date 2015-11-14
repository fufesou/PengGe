/**
 * @file client_account.h
 * @brief  This file contains some client request operations for account.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  周三 2015-11-11 19:10:06 中国标准时间
 */

#ifndef _CLIENT_ACCOUNT_H
#define _CLIENT_ACCOUNT_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  am_account_create_request This function start a creating account session.
 *
 * @param inmsg The message of user, such as telephone number.
 * @param inmsglen The length of income message.
 * @param userid This useris can be any value.
 * @param outmsg The request message for server.
 * @param outmsglen This value-result argument is used to show the length of output message.
 *
 * @return   0 if generate request succeed, 1 if fail.
 *
 * @note We support only telephone account creation for now.
 */
int am_account_create_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, __inout uint32_t* outmsglen);

int am_account_login_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, __inout uint32_t* outmsglen);
int am_account_inquire_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, __inout uint32_t* outmsglen);
int am_account_changeusername_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, __inout uint32_t* outmsglen);
int am_account_changepasswd_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, __inout uint32_t* outmsglen);
int am_account_changegrade_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, __inout uint32_t* outmsglen);

#ifdef __cplusplus
}
#endif

#endif //CLIENT_ACCOUNT_H
