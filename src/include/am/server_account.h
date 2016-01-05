/**
 * @file server_account.h
 * @brief  This file contains some server account operations.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  Fri 2015-11-20 18:30:07 (+0800)
 */

#ifndef _SERVER_ACCOUNT_H
#define _SERVER_ACCOUNT_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief  am_server_account_init will initialize mutex and thread, which help manage the loged in accounts.
 *
 * @return  0 if succeed, 1 if fail. 
 */
JXIOT_API int am_server_account_init(void);

/** am_server_account_clear will be added in am_server_account_init, user need not call
 * am_server_account_clear manually. 
 *
 * Thus, the am_server_account_clear is declared as 
 * static function in the source file.
 */

#ifdef __cplusplus
}
#endif

#endif //SERVER_ACCOUNT_H
