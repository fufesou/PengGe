/**
 * @file account.h
 * @brief  This file describe the accout structure and its basic operations.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-09
 * @modified  Mon 2015-11-09 22:06:20 (+0800)
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

#ifdef __cplusplus
}
#endif

#endif //ACCOUNT_H
