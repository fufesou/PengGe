/**
 * @file client_account.h
 * @brief  This file contains some client request operations for account. 'amc_' means accout management client.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  Mon 2015-11-09 22:57:33 (+0800)
 */

#ifndef _CLIENT_ACCOUNT_H
#define _CLIENT_ACCOUNT_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  amc_account_create This function will generate the request message for creating account. The message will be transport to the server and handled by server.
 *
 * @param request the generated message.
 * @param datalen value-result argument that stands for the length of request.
 */
void amc_account_create(char* request, __inout int* datalen);

/**
 * @brief  amc_account_login 
 *
 * @param username
 * @param password
 * @param request
 * @param datalen
 */
void amc_account_login(const char* username, const char* password, char* request, __inout int* datalen);

void amc_account_inquire(uint32_t id, char* request, __inout int* datalen);

/**
 * @brief  amc_account_changeusername 
 *
 * @param id
 * @param password
 * @param newusername
 * @param request
 * @param datalen
 */
void amc_account_changeusername(uint32_t id, const char* password, const char* newusername, char* request, __inout int* datalen);

/**
 * @brief  amc_account_changepasswd 
 *
 * @param id
 * @param oldpasswd
 * @param newpasswd
 * @param request
 * @param datalen
 */
void amc_account_changepasswd(uint32_t id, const char* oldpasswd, const char* newpasswd, char* request, __inout int* datalen);


void amc_account_changegrade(uint32_t id, char* request, __inout int* datalen);

#ifdef __cplusplus
}
#endif

#endif //CLIENT_ACCOUNT_H
