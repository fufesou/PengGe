/**
 * @file account.h
 * @brief  This file describe the accout structure and its basic operations.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  Fri 2015-11-20 18:30:39 (+0800)
 */

#ifndef _ACCOUNT_H
#define _ACCOUNT_H

#include    "account_macros.h"


#pragma pack(4)
struct account_basic_t {
	uint8_t grade;
	uint32_t id;
    char tel[ACCOUNT_TEL_LEN];
	char username[ACCOUNT_USERNAME_LEN];
};
#pragma pack()

#ifdef __cplusplus
extern "C"
{
#endif


DECLARE_ACCOUNT_METHOD(account_create)
DECLARE_ACCOUNT_METHOD(account_verify)
DECLARE_ACCOUNT_METHOD(account_login)
DECLARE_ACCOUNT_METHOD(account_logout)
DECLARE_ACCOUNT_METHOD(account_changeusername)
DECLARE_ACCOUNT_METHOD(account_changepasswd)
DECLARE_ACCOUNT_METHOD(account_changegrade)


typedef int (*pfunc_react)(char*, char*, __inout uint32_t*);
typedef int (*pfunc_reply)(char*, const void*, uint32_t, char*, __inout uint32_t*);

/**
 * @brief  account_method_t This struct is used for method name-func mapping. This is useful when creating message request and replying request and reacting to the reply.
 */
struct account_method_t { 
    const char* methodname;
    pfunc_reply reply;
    pfunc_react react;
};

/**
 * @brief am_send_randcode
 * @param tel
 * @param randcode
 */
void am_send_randcode(const char* tel, const char* randcode);

/**
 * @brief  am_method_sort  This function sort s_methodarr, making the performance much better.
 */
void am_method_sort(void);

/**
 * @brief  am_method_get  This function returns the method name that registered in REGISTER_ACCOUNT_PROCESS(proc).
 *
 * @param methodid
 *
 * @return   NULL if methodid is invalid, the pointer to the method struct otherwise.
 */
const struct account_method_t* am_method_get(uint32_t methodid);

/**
 * @brief am_method_getid  This function returns the 
 *
 * @param methodname
 *
 * @return  INVALID_METHOD_ID if methodname do not exist, the actual index as id otherwise. 
 *
 * @note The registered method names should better be sorted. And then, this method should better find the method id using binary search.
 *
 * @sa am_method_sort
 */
uint32_t am_method_getid(const char* methodname);

#ifdef __cplusplus
}
#endif

#endif //ACCOUNT_H
