/**
 * @file server_account.h
 * @brief  This file contains some server account operations. 'ams_' means account management server.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  Mon 2015-11-09 22:30:07 (+0800)
 */

#ifndef _SERVER_ACCOUNT_H
#define _SERVER_ACCOUNT_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  ams_account_create_tmp This function creates a temporary account that waiting for verify. And if verification passed, the account will become a normal one.
 *
 * @param request
 * @param outmsg
 * @param outmsglen
 *
 * @return 0 if succeed, 1 if fail.
 */
int ams_account_create_tmp(char* request, char* outmsg, __inout int* outmsglen);

/**
 * @brief  ams_account_create_verify If succeed, this function will reset the account password to a randomly generated string.
 *
 * @param inmsg
 * @param outmsg
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int ams_account_create_verify(char* inmsg, char* outmsg, __inout int* outmsglen);

#ifdef __cplusplus
}
#endif

#endif //SERVER_ACCOUNT_H
