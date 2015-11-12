/**
 * @file account_config.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-11
 * @modified  周四 2015-11-12 18:32:22 中国标准时间
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <stdint.h>
#include  <string.h>
#include    "utility_wrap.h"
#include    "account_config.h"

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifdef __cplusplus
extern "C" {
#endif

static FILE* s_cfgfile = NULL;

static int s_account_find_common(void* keydata, int cmpcount, size_t dataoffset, struct account_data_t* account);

#ifdef __cplusplus
}
#endif

int s_account_find_common(void* keydata, int cmpcount, size_t dataoffset, struct account_data_t* account)
{
    fseek(s_cfgfile, 0, SEEK_SET);
    while (!feof(s_cfgfile)) {
        if (fread(account, sizeof(*account), 1, s_cfgfile) == 1) {
            if (memcmp((char*)account + dataoffset, keydata, cmpcount) == 0) {
                return 0;
            }
        } else if (ferror(s_cfgfile)) {
            fprintf(stderr, "file - %s, line - %d, read file error.\n", __FILE__, __LINE__);
        }
    }
    return 1;
}

int am_account_find_id(uint32_t id, struct account_data_t* account)
{
    return s_account_find_common(&id, sizeof(id), offsetof(struct account_data_t, id), account);
}

int am_account_find_username(const char* username, struct account_data_t* account)
{
    if (strlen(username) >= sizeof(account->username)) {
        fprintf(stderr, "account find error, the size of key is too large.\n");
        return 1;
    }
    return s_account_find_common((void*)username, strlen(username) + 1, offsetof(struct account_data_t, username), account);
}

int am_account_find_tel(const char* tel, struct account_data_t* account)
{
    if (strlen(tel) >= sizeof(account->tel)) {
        fprintf(stderr, "account find error, the size of key is too large.\n");
        return 1;
    }
    return s_account_find_common((void*)tel, strlen(tel) + 1, offsetof(struct account_data_t, tel), account);
}


void am_account_config_init(void)
{
    if (cs_fopen(&s_cfgfile, "account.txt", "ab+") != 0) {
        printf("open file error.\n");
        /** call safe exit(1) */
    }
}

void am_account_config_clear(void)
{
    if (s_cfgfile != NULL) {
        if (fclose(s_cfgfile) != 0) {
            printf("close file error.\n");
        }
    }
}

int am_account_print(FILE* streamptr, const struct account_data_t* account)
{
    return fprintf(
                streamptr,
                "account info: id - %d, name - %s, passwd - %s, grade - %d.\n",
                account->id,
                account->username,
                account->passwd,
                account->grade);
}

int am_account_write(const struct account_data_t* account)
{
    if (fseek(s_cfgfile, 0, SEEK_END) != 0) {
        fprintf(stderr, "write new account error, call fseek fail.\n");
        return 1;
    }
    if (fwrite((void*)account, sizeof(struct account_data_t), 1, s_cfgfile) != 1) {
        fprintf(stderr, "write new account error, fwrite fail.\n");
        return 1;
    }

    if (fflush(s_cfgfile)) {
        fprintf(stderr, "file - %s, line - %d: fflush error.", __FILE__, __LINE__);
    }
    return 0;
}


