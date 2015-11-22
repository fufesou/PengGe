/**
 * @file account_file.h
 * @brief  This file process account writing and finding operations. A crude mutex protection is used here to promise atomic file operation.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-11
 * @modified  Sun 2015-11-22 20:08:03 (+0800)
 */

#ifndef _ACCOUNT_FILE_H
#define _ACCOUNT_FILE_H

struct account_data_t {
    uint8_t		grade;
    uint32_t	id;
    char		tel[ACCOUNT_TEL_LEN];
    char		username[ACCOUNT_USERNAME_LEN];
    char		passwd[ACCOUNT_PASSWD_LEN];
};

#ifdef __cplusplus
extern "C" {
#endif


void am_account_config_init(void);
int am_account_print(FILE* streamptr, const struct account_data_t* account);
int am_account_data2basic(const struct account_data_t* data, struct account_basic_t* basic);

/**
 * @brief am_account_write
 * @param account
 * @return 0 if succeed, 1 if fail.
 */
int am_account_write(const struct account_data_t* account);

/**
 * @brief am_account_find_id
 * @param id
 * @param account
 * @return 1 if account is found, or return 0.
 *
 * @note atomic write should be guaranteed here.
 */
int am_account_find_id(uint32_t id, struct account_data_t* account);

/**
 * @brief  am_account_find_tel 
 *
 * @param tel
 * @param account
 *
 * @return 1 if account is found, or return 0.
 */
int am_account_find_tel(const char* tel, struct account_data_t* account);

/**
 * @brief  am_account_find_login This function find the account with login information.
 *
 * The login information here contains: tel--passwd or email--passwd
 *
 * @param login
 * @param account
 *
 * @return 1 if account is found, or return 0.
 */
int am_account_find_login(const char* login, struct account_data_t* account);

/**
 * @brief  am_account_update If account exist already, the data will be updated, otherwise new account will be appended. 
 * The only way to find the corresponding account is to match the id.
 *
 * @param account
 *
 * @return   
 */
int am_account_update(const struct account_data_t* account);

#ifdef __cplusplus
}
#endif


#endif //ACCOUNT_FILE_H
