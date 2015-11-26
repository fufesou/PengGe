/**
 * @file config_macros.h
 * @brief  This file contains the macros which may be replaces by varialbes whose values come from config.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-14
 * @modified  Wed 2015-11-25 19:09:02 (+0800)
 */

#ifndef _CONFIG_MACROS_H
#define _CONFIG_MACROS_H


/**************************************************
 **           client server block                **
 **************************************************/
#define MAX_MSG_LEN				1024
#define MAX_BUF_LEN				2048
#define NUM_THREAD				8
#define SERVER_POOL_NUM_ITEM	64


/**************************************************
 **         account management block             **
 **************************************************/
#define ACCOUNT_PASSWD_LEN		32
#define ACCOUNT_USERNAME_LEN	64
#define ACCOUNT_TEL_LEN			16
#define ACCOUNT_USERNUM_LEN		16

#endif //CONFIG_MACROS_H
