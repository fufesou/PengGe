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
 * @modified  Sat 2015-11-14 17:29:02 (+0800)
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
 * @param inmsg inmsg The format of inmsg here is:
 * --------------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | (*) process id(int32_t) | tel(char*) | ... |
 * --------------------------------------------------------------------------------------------
 *
 * @param inmsglen The length of income message.
 * @param userid This useris can be any value.
 * @param outmsg The format of outmsg here is: 
 * -------------------------------------------------------------------------------------
 * | struct csmsg_header |  user id(uint32_t) | process id(int32_t) | tel(char*) | ... |
 * -------------------------------------------------------------------------------------
 *
 * @param outmsglen The buffer size of outmsg.
 *
 * @return   0 if generate request succeed, 1 if fail.
 *
 * @note We support only telephone account creation for now.
 */
int am_account_create_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, uint32_t outmsglen);

/**
 * @brief  am_account_login_request 
 *
 * @param inmsg The format of inmsg here is: 
 * ------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | (*) process id(int32_t) | tel or username(char*) | passwd(char*) | ... |
 * ------------------------------------------------------------------------------------------------------------------------
 *
 * @param inmsglen
 * @param userid
 * @param outmsg The format of outmsg here is: 
 * --------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | process id(int32_t) | tel or username(char*) | passwd(char*) | ... |
 * --------------------------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_login_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, uint32_t outmsglen);

/**
 * @brief  am_account_changeusername_request
 *
 * @param inmsg inmsg The format of inmsg here is: 
 * -------------------------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | (*) process id(int32_t) | old username(char*) | passwd(char*) | new username(char*) | ... |
 * -------------------------------------------------------------------------------------------------------------------------------------------
 *
 * @param inmsglen
 * @param userid
 * @param outmsg The format of outmsg is
 * -----------------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | old username(char*) | passwd(char*) | new username(char*) | ... |
 * -----------------------------------------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changeusername_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, uint32_t outmsglen);

/**
 * @brief  am_account_changepasswd_request 
 *
 * @param inmsg
 * @param inmsglen
 * @param userid
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_changepasswd_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, uint32_t outmsglen);

/**
 * @brief  am_account_changegrade_request 
 *
 * @param inmsg
 * @param inmsglen
 * @param userid
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_changegrade_request(const char* inmsg, uint32_t inmsglen, uint32_t userid, char* outmsg, uint32_t outmsglen);

/**
 * @brief  am_account_inquire_request 
 *
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_inquire_request(char* outmsg, uint32_t outmsglen);


#ifdef __cplusplus
}
#endif

#endif //CLIENT_ACCOUNT_H
