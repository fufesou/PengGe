/**
 * @file account_config.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-11
 * @modified  Sat 2015-11-14 11:18:27 (+0800)
 */

#ifdef WIN32
#include  <process.h>
#else
#include  <errno.h>
#include  <pthread.h>
#include  <unistd.h>
#include  <sys/time.h>
#define THREAD_IDEL_TIMESLICE_MS 20
#endif

#include  <time.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <stdint.h>
#include  <string.h>
#include    "config_macros.h"
#include    "error.h"
#include    "lightthread.h"
#include    "utility_wrap.h"
#include    "account_macros.h"
#include    "account.h"
#include    "account_config.h"

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifdef __cplusplus
extern "C" {
#endif

static FILE* s_cfgfile = NULL;
static csmutex_t s_filemutex;

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

int am_account_find_tel_username(const char* tel_username, struct account_data_t* account)
{
    static int offset_tel = offsetof(struct account_data_t, tel);
    static int offset_username = offsetof(struct account_data_t, username);
    size_t keylen = strlen(tel_username) + 1;

    if ((keylen - 1) >= sizeof(account->username) && (keylen - 1) >= sizeof(account->tel)) {
        return 1;
    }
    if (strlen(tel_username) > sizeof(account->username)) {
        return am_account_find_tel(tel_username, account);
    }
    if (strlen(tel_username) > sizeof(account->tel)) {
        return am_account_find_username(tel_username, account);
    }

    csmutex_lock(s_filemutex);
    fseek(s_cfgfile, 0, SEEK_SET);
    while (!feof(s_cfgfile)) {
        if (fread(account, sizeof(*account), 1, s_cfgfile) == 1) {
            if (
                    memcmp((char*)account + offset_tel, tel_username, keylen) == 0 ||
                    memcmp((char*)account + offset_username, tel_username, keylen) == 0 ) {
                csmutex_unlock(s_filemutex);
                return 0;
            }
        } else if (ferror(s_cfgfile)) {
            fprintf(stderr, "file - %s, line - %d, read file error.\n", __FILE__, __LINE__);
        }
    }
    csmutex_unlock(s_filemutex);
    return 1;
}

void am_account_config_init(void)
{
    int errcode = 0;
    if (cs_fopen(&s_cfgfile, "account.txt", "ab+") != 0) {
        errcode = 1;
        csfatal_ext(&errcode, cserr_exit, "open file error.\n");
    }

    s_filemutex = csmutex_create();
}

void am_account_config_clear(void)
{
    if (s_cfgfile != NULL) {
        if (fclose(s_cfgfile) != 0) {
            fprintf(stderr, "close file error.\n");
        }
    }

    csmutex_destroy(s_filemutex);
}

int am_account_data2basic(const struct account_data_t* data, struct account_basic_t* basic)
{
    return cs_memcpy(basic, sizeof(basic), data, sizeof(basic));
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
    csmutex_lock(s_filemutex);
    if (fseek(s_cfgfile, 0, SEEK_END) != 0) {
        fprintf(stderr, "write new account error, call fseek fail.\n");
        csmutex_unlock(s_filemutex);
        return 1;
    }
    if (fwrite((void*)account, sizeof(struct account_data_t), 1, s_cfgfile) != 1) {
        fprintf(stderr, "write new account error, fwrite fail.\n");
        csmutex_unlock(s_filemutex);
        return 1;
    }
    csmutex_unlock(s_filemutex);

    if (fflush(s_cfgfile)) {
        fprintf(stderr, "file - %s, line - %d: fflush error.", __FILE__, __LINE__);
    }

    return 0;
}


