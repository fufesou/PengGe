/**
 * @file account_config.h
 * @brief  This file process account writing and finding operations.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-11
 * @modified  周三 2015-11-11 17:53:20 中国标准时间
 */

#ifndef _ACCOUNT_CONFIG_H
#define _ACCOUNT_CONFIG_H

struct account_data_t {
    uint8_t grade;
    uint32_t id;
    char username[64];
    char passwd[32];
};

#ifdef __cplusplus
extern "C" {
#endif


void am_account_config_init(void);
void am_account_config_clear(void);
int am_account_print(FILE* streamptr, const struct account_data_t* account);

/**
 * @brief am_account_write
 * @param account
 * @return
 *
 * @todo atomic write should be guaranteed here.
 */
int am_account_write(const struct account_data_t* account);

/**
 * @brief am_account_find_id
 * @param id
 * @param account
 * @return
 *
 * @note atomic write should be guaranteed here.
 */
int am_account_find_id(uint32_t id, struct account_data_t* account);

/**
 * @brief am_account_find_username
 * @param username
 * @param account
 * @return
 *
 * @todo atomic write should be guaranteed here.
 */
int am_account_find_username(const char* username, struct account_data_t* account);

#ifdef __cplusplus
}
#endif


#endif //ACCOUNT_CONFIG_H
