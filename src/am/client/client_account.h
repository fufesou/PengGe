/**
 * @file client_account.h
 * @brief  This file contains some client request operations for account.
 * Some message formats are described in the document:
 * -----------------------------------------------
 * | data name(type) | (*) data name(type) | ... |
 * -----------------------------------------------
 * 1. The data name is the name of this segment, such as 'username'.
 * 2. The type describe the segement's type, such as 'int8'.
 * 3. (*) means this segement is not filled with data by now.
 * 4. ... means unused remainder place.

 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  Mon 2015-11-16 01:51:34 (+0800)
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
 * @param tel The telphone number.
 * @param outmsg The format of outmsg here is: 
 * ------------------------------------------------------------------
 * | (*) user id(uint32_t) | process id(int32_t) | tel(char*) | ... |
 * ------------------------------------------------------------------
 *
 * @param outmsglen The buffer size of outmsg.
 *
 * @return   0 if succeed, 1 if fail.
 *
 * @note We support only telephone account creation for now.
 */
int am_account_create_request(const char* tel, char* outmsg, uint32_t* outmsglen);

/**
 * @brief  am_account_login_request 
 *
 * @param username_tel
 * @param passwd
 * @param outmsg The format of outmsg here is: 
 * ----------------------------------------------------------------------------------------------
 * | (*) user id(uint32_t) | process id(int32_t) | tel or username(char*) | passwd(char*) | ... |
 * ----------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_login_request(const char* username_tel, const char* passwd, char* outmsg, uint32_t* outmsglen);

/**
 * @brief  am_account_changeusername_request
 *
 * @param username_old
 * @param passwd
 * @param username_new
 * @param outmsg The format of outmsg is
 * -------------------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | old username(char*) | passwd(char*) | new username(char*) | ... |
 * -------------------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changeusername_request(
			const char* username_old,
			const char* passwd,
			const char* username_new,
			char* outmsg,
            uint32_t* outmsglen);

/**
 * @brief  am_account_changepasswd_request 
 *
 * @param username
 * @param passwd_old
 * @param passwd_new
 * @param outmsg The format of outmsg is
 * -----------------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | username(char*) | old passwd(char*) | new passwd(char*) | ... |
 * -----------------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changepasswd_request(
			const char* username,
			const char* passwd_old,
			const char* passwd_new,
			char* outmsg,
            uint32_t* outmsglen);

/**
 * @brief  am_account_changegrade_request 
 *
 * @param username
 * @param passwd
 * @param grade
 * @param outmsg The format of outmsg is
 * -----------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | username(char*) | passwd(char*) | grade(uint32_t) | ... |
 * -----------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_changegrade_request(
			const char* username,
			const char* passwd,
			uint32_t grade,
			char* outmsg,
            uint32_t* outmsglen);

/**
 * @brief  am_account_inquire_request 
 *
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_inquire_request(char* outmsg, uint32_t* outmsglen);


#ifdef __cplusplus
}
#endif

#endif //CLIENT_ACCOUNT_H
