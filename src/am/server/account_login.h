/**
 * @file account_login.h
 * @brief  This file defines the login account operations for server.
 * @author cxl, <shuanlgongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-16
 * @modified  Mon 2015-11-16 19:43:54 (+0800)
 */

#ifndef _ACCOUNT_LOGIN_H
#define _ACCOUNT_LOGIN_H

#pragma pack(4)
struct account_login_t {
	struct account_data_t account;

	void* data_verification;
    uint32_t len_verification;
};

struct list_login_t {
    struct list_head listnode;
    struct account_login_t* account_sock;
};

#pragma pack()


#ifdef __cplusplus
extern "C"
{
#endif

void am_login_add(struct list_head* node_head, const struct account_data_t* account, const void* data_verification, uint32_t len_verification);

/**
 * @brief  am_login_remove 
 *
 * @param node_head
 * @param id_account
 * @param data_verification
 * @param len_verification
 *
 * @return   
 * 1. 0 if account exists and is removed.
 * 2. 1 if account not exists.
 * 3. 2 if verification fail.
 */
int am_login_remove(struct list_head* node_head, uint32_t id_account, const void* data_verification, uint32_t len_verification);

/**
 * @brief  am_login_tryadd This function will add new login account to login account list if previous same login have not happened.
 *
 * @param node_head
 * @param account
 * @param data_verification
 * @param len_verification
 *
 * @return
 * 1. 0 if no login of this account happened.
 * 2. 1 if login of the account happened on other socket.
 * 3. 2 if login  happened on the same socket.
 */
int am_login_tryadd(struct list_head* node_head, const struct account_data_t* account, const void* data_verification, uint32_t len_verification);

struct account_login_t* am_login_find(const struct list_head* node_head, uint32_t id_account);

/**
 * @brief  am_login_exist 
 *
 * @param node_head
 * @param account
 * @param data_verification
 * @param len_verification
 *
 * @return   
 * 1. 0 if no login of this account happened.
 * 2. 1 if login of the account happened on other socket.
 * 3. 2 if login  happened on the same socket.
 */
int am_login_exist(const struct list_head* node_head, const struct account_data_t* account, const void* data_verification, uint32_t len_verification);


int am_login_write(const struct list_head* node_head);

void am_login_clear(struct list_head* node_head);

#ifdef __cplusplus
}
#endif

#endif //ACCOUNT_LOGIN_H
