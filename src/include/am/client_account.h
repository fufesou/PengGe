/**
 * @file client_account.h
 * @brief  This file contains some client request operations for account.
 *
 * Some message formats are described in the document: \n
 * ---------------------------------------------------------------------------\n
 * | data name(type) | (*) data name(type) | ... |                            \n
 * ---------------------------------------------------------------------------\n
 * - The data name is the name of this segment, such as 'username'.
 * - The type describe the segement's type, such as 'int8'.
 * - (*) means this segement is not filled with data by now.
 * - ... means unused remainder place.

 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  Sun 2015-12-06 18:21:47 (+0800)
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
 * @param usernum The usernum should better be provided by user's qq number. If This parameter is NULL,
 * or usernum is not a valid number string, the server will provide a number with at least 12 character length.
 * @param tel The telphone number.
 * @param outmsg The format of outmsg here is: \n
 * ---------------------------------------------------------------------------------------------------------\n
 * | process id(uint32_t) | usernum(char*) | tel(char*) | usernum(char*) | ... |                            \n
 * ---------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen The buffer size of outmsg.
 *
 * @return   0 if succeed, 1 if fail.
 *
 * @note We support only telephone account creation for now.
 */
JXIOT_API int am_account_create_request(const char* usernum, const char* tel, char* outmsg, uint32_t* outmsglen);

/**
 * @brief  am_account_verify_request 
 *
 * @param tel       The telephone number.
 * @param randcode  The random verification code generated by the server.
 * @param outmsg    The format of outmsg here is: \n
 * -----------------------------------------------------------------------------------------\n
 * | process id(uint32_t) | tel(char*) | randcode(char*) | ... |                            \n
 * -----------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 */
JXIOT_API int am_account_verify_request(const char* tel, const char* randcode, char* outmsg, uint32_t* outmsglen);

/**
 * @brief  am_account_login_request 
 *
 * @param username_tel
 * @param passwd
 * @param outmsg The format of outmsg here is: \n
 * --------------------------------------------------------------------------------------------\n
 * | process id(uint32_t) | keywords(char*) | passwd(char*) | ... |                            \n
 * --------------------------------------------------------------------------------------------\n
 *
 *  The keywords can be the telephone and the email.
 *
 * @param outmsglen
 *
 * @return   
 */
JXIOT_API int am_account_login_request(const char* keywords, const char* passwd, char* outmsg, uint32_t* outmsglen);

/**
 * @brief  am_account_logout_request 
 *
 * @param outmsg The format of outmsg here is: \n
 * ------------------------------------------------------------------------------\n
 * | process id(uint32_t) | user id(uint32_t) | ... |                            \n
 * ------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 */
JXIOT_API int am_account_logout_request(char* outmsg, uint32_t* outmsglen);

/**
 * @brief  am_account_changeusername_request
 *
 * @param passwd
 * @param username_new
 * @param outmsg The format of outmsg is \n
 * --------------------------------------------------------------------------------------------------------------------------------\n
 * | process id(uint32_t) | user id(uint32_t) | passwd(char*) | new username(char*) | ... |                                        \n
 * --------------------------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 */
JXIOT_API int am_account_changeusername_request(
            const char* passwd,
            const char* username_new,
            char* outmsg,
            uint32_t* outmsglen);

/**
 * @brief  am_account_changepasswd_request 
 *
 * @param passwd_old
 * @param passwd_new
 * @param outmsg The format of outmsg is \n
 * ----------------------------------------------------------------------------------------------------------------------------------\n
 * | process id(uint32_t) | user id(uint32_t) | old passwd(char*) | new passwd(char*) | ... |                                        \n
 * ----------------------------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 */
JXIOT_API int am_account_changepasswd_request(
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
 * @param outmsg The format of outmsg is \n
 * ---------------------------------------------------------------------------------------------------------------\n
 * | process id(uint32_t) | user id(uint32_t) | passwd(char*) | grade(uint8_t) | ... |                            \n
 * ---------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
JXIOT_API int am_account_changegrade_request(
            const char* passwd,
            uint8_t grade,
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
JXIOT_API int am_account_inquire_request(char* outmsg, uint32_t* outmsglen);


#ifdef __cplusplus
}
#endif

#endif //CLIENT_ACCOUNT_H
