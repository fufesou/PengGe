/**
 * @file account.h
 * @brief  This file describe the accout structure and its basic operations.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  周二 2015-11-10 19:44:56 中国标准时间
 */

#ifndef _ACCOUNT_H
#define _ACCOUNT_H

#ifdef __cplusplus
extern "C"
{
#endif

struct account{
	uint8_t grade;
	uint32_t id;
	char username[64];
};

typedef int procfunc(char*, int, char*, __inout int*);

DECLARE_ACCOUNT_METHOD(account_create)
DECLARE_ACCOUNT_METHOD(account_create_verify)
DECLARE_ACCOUNT_METHOD(account_login)
DECLARE_ACCOUNT_METHOD(account_inquire)
DECLARE_ACCOUNT_METHOD(account_changeusername)
DECLARE_ACCOUNT_METHOD(account_changepasswd)
DECLARE_ACCOUNT_METHOD(account_changegrade)

struct account_method_t { 
    const char* methodname, 
    procfunc* reply, 
    procfunc* react, 
};

/**
 * @brief  am_method_sort  This function sort s_methodarr, making the performance much better.
 */
void am_method_sort(void);

/**
 * @brief  am_method_getname  This function returns the method name that registered in REGISTER_ACCOUNT_PROCESS(proc).
 *
 * @param methodid
 *
 * @return   NULL if methodid is invalid, the pointer to the method struct otherwise.
 */
const struct account_method_t* am_method_getname(uint32_t methodid);

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
