/**
 * @file account_file.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-11
 * @modified  Thu 2015-11-26 22:41:31 (+0800)
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
#include  <semaphore.h>
#include    "config_macros.h"
#include    "list.h"
#include    "clearlist.h"
#include    "error.h"
#include    "lightthread.h"
#include    "utility_wrap.h"
#include    "account_macros.h"
#include    "account.h"
#include    "account_file.h"

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifdef __cplusplus
extern "C" {
#endif

static const char* s_cfgfile = "account.txt";
static FILE* s_fpcfg = NULL;
static csmutex_t s_mutex_file;

static int s_account_find_common(void* keydata, int cmpcount, size_t dataoffset, struct account_data_t* account);

static void s_am_account_config_clear(void* unused);

#ifdef __cplusplus
}
#endif

int s_account_find_common(void* keydata, int cmpcount, size_t dataoffset, struct account_data_t* account)
{
    csmutex_lock(&s_mutex_file);
    fseek(s_fpcfg, 0, SEEK_SET);
    while (!feof(s_fpcfg)) {
        if (fread(account, sizeof(*account), 1, s_fpcfg) == 1) {
            if (memcmp((char*)account + dataoffset, keydata, cmpcount) == 0) {
                csmutex_unlock(&s_mutex_file);
                return 1;
            }
        } else if (ferror(s_fpcfg)) {
            fprintf(stderr, "file - %s, line - %d, read file error.\n", __FILE__, __LINE__);
        }
    }
    csmutex_unlock(&s_mutex_file);

    return 0;
}

int am_account_find_id(uint32_t id, struct account_data_t* account)
{
	size_t offset = offsetof(struct account_basic_t, id) + offsetof(struct account_data_t, data_basic);
    return s_account_find_common(&id, sizeof(id), offset, account);
}

int am_account_find_tel(const char* tel, struct account_data_t* account)
{
    if (strlen(tel) >= sizeof(account->data_basic.tel)) {
        fprintf(stderr, "account find error, the size of key is too large.\n");
        return 0;
    }
	size_t offset = offsetof(struct account_basic_t, tel) + offsetof(struct account_data_t, data_basic);
    return s_account_find_common((void*)tel, strlen(tel) + 1, offset, account);
}

int am_account_find_usernum(const char* usernum, struct account_data_t* account)
{
    if (strlen(usernum) >= sizeof(account->data_basic.usernum)) {
        fprintf(stderr, "account find error, the size of key is too large.\n");
        return 0;
    }
	size_t offset = offsetof(struct account_basic_t, usernum) + offsetof(struct account_data_t, data_basic);
    return s_account_find_common((void*)usernum, strlen(usernum) + 1, offset, account);
}

int am_account_find_login(const char* login, struct account_data_t* account)
{
	size_t len_first = strlen(login);
	size_t len_second = strlen(login + len_first + 1); 

    csmutex_lock(&s_mutex_file);
    fseek(s_fpcfg, 0, SEEK_SET);
    while (!feof(s_fpcfg)) {
        if (fread(account, sizeof(*account), 1, s_fpcfg) == 1) {
			if ((memcmp(account->data_basic.tel, login, len_first) == 0) ||
						(memcmp(account->data_basic.usernum, login, len_first) == 0)) {
				csmutex_unlock(&s_mutex_file);
                if (memcmp(account->passwd, login + len_first + 1, len_second) == 0) {
                    return 1;
                } else {
                    return 2;
                }
			}
        } else if (ferror(s_fpcfg)) {
            fprintf(stderr, "file - %s, line - %d, read file error.\n", __FILE__, __LINE__);
        }
    }
    csmutex_unlock(&s_mutex_file);
    return 0;
}

void am_account_config_init(void)
{
    int errcode = 0;
    static int s_inited = 0;

    if (s_inited) {
        return;
    }

    if (cs_fopen(&s_fpcfg, s_cfgfile, "ab+") != 0) {
        errcode = 1;
        csfatal_ext(&errcode, cserr_exit, "open file error.\n");
    }

    s_mutex_file = csmutex_create();

    csclearlist_add(s_am_account_config_clear, NULL);
    s_inited = 1;
}

void s_am_account_config_clear(void* unused)
{
    (void)unused;
    if (s_fpcfg != NULL) {
        if (fclose(s_fpcfg) != 0) {
            fprintf(stderr, "close file error.\n");
        }
    }

    csmutex_destroy(&s_mutex_file);
}

int am_account_data2basic(const struct account_data_t* data, struct account_basic_t* basic)
{
    return cs_memcpy(basic, sizeof(basic), &data->data_basic, sizeof(basic));
}

int am_account_print(FILE* streamptr, const struct account_data_t* account)
{
    return fprintf(
                streamptr,
                "account info: id - %d, name - %s, passwd - %s, grade - %d.\n",
                account->data_basic.id,
                account->data_basic.username,
                account->passwd,
                account->data_basic.grade);
}

int am_account_write(const struct account_data_t* account)
{
    if (csmutex_lock(&s_mutex_file) != 0) {
        fprintf(stderr, "file- %s, line- %d, csmutex_lock error.\n", __FILE__, __LINE__);
    }
    if (fseek(s_fpcfg, 0, SEEK_END) != 0) {
        fprintf(stderr, "write new account error, call fseek fail.\n");
        csmutex_unlock(&s_mutex_file);
        return 1;
    }
    if (fwrite((void*)account, sizeof(struct account_data_t), 1, s_fpcfg) != 1) {
        fprintf(stderr, "write new account error, fwrite fail.\n");
        csmutex_unlock(&s_mutex_file);
        return 1;
    }

    if (fflush(s_fpcfg)) {
        fprintf(stderr, "file - %s, line - %d: fflush error.", __FILE__, __LINE__);
    }
    csmutex_unlock(&s_mutex_file);

    return 0;
}

int am_account_update(const struct account_data_t* account)
{
	FILE* fptmp = NULL;
	const char* tmpname = "tmp.txt";
    int errcode = 0;
	struct account_data_t account_tmp;

	cs_fopen(&fptmp, tmpname, "ab+");
    if (fwrite(account, sizeof(*account), 1, fptmp) != 1) {
        fprintf(stderr, "update account error, cannot write account data to tmp file.\n");
        fclose(fptmp);
        return 1;
    }

    csmutex_lock(&s_mutex_file);
    rewind(s_fpcfg);
    while (!feof(s_fpcfg)) {
        if (fread(&account_tmp, sizeof(account_tmp), 1, s_fpcfg) == 1) {
            if (account_tmp.data_basic.id != account->data_basic.id) {
                if (fwrite(&account_tmp, sizeof(account_tmp), 1, fptmp) != 1) {
                    fprintf(stderr, "update account error, cannot write account data to tmp file.\n");
                    fclose(fptmp);
                    return 1;
                }
            }
        }
    }

    fclose(fptmp);
	fclose(s_fpcfg);
	remove(s_cfgfile);
	rename(tmpname, s_cfgfile);

    if (cs_fopen(&s_fpcfg, s_cfgfile, "ab+") != 0) {
        errcode = 1;
        csfatal_ext(&errcode, cserr_exit, "update file fatal error, open file error.\n");
    }

    csmutex_unlock(&s_mutex_file);

	return 0;
}
