/**
 * @file account_file.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-11
 * @modified  Sun 2015-12-06 18:04:32 (+0800)
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

#ifndef _MSC_VER /* *nix */
#include  <semaphore.h>
#endif

#include  <time.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/list.h"
#include    "common/clearlist.h"
#include    "common/error.h"
#include    "common/lightthread.h"
#include    "common/utility_wrap.h"
#include    "am/account_macros.h"
#include    "am/account.h"
#include    "am/account_file.h"

#ifndef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#ifdef __cplusplus
extern "C" {
#endif

static char s_cfgfile[ACCOUNT_FILE_NUM][16];
static FILE* s_fpcfg[ACCOUNT_FILE_NUM];
static jxmutex_t s_mutex_file[ACCOUNT_FILE_NUM];

static int s_account_find_common(uint32_t fileid, void* keydata, int cmpcount, size_t dataoffset, struct account_data_t* account);

static void s_am_account_config_clear(void* unused);

#ifdef __cplusplus
}
#endif


int s_account_find_common(uint32_t fileid, void* keydata, int cmpcount, size_t dataoffset, struct account_data_t* account)
{
    jxmutex_lock(&s_mutex_file[fileid]);
    if (s_fpcfg[fileid] == 0) {
        return 0;
    }
    fseek(s_fpcfg[fileid], 0, SEEK_SET);
    while (!feof(s_fpcfg[fileid])) {
        if (fread(account, sizeof(*account), 1, s_fpcfg[fileid]) == 1) {
            if (memcmp((char*)account + dataoffset, keydata, cmpcount) == 0) {
                jxmutex_unlock(&s_mutex_file[fileid]);
                return 1;
            }
        } else if (ferror(s_fpcfg[fileid])) {
            fprintf(stderr, "file - %s, line - %d, read file error.\n", __FILE__, __LINE__);
        }
    }
    jxmutex_unlock(&s_mutex_file[fileid]);

    return 0;
}

int am_account_find_id(uint32_t id, struct account_data_t* account)
{
    size_t offset = offsetof(struct account_basic_t, id) + offsetof(struct account_data_t, data_basic);
    return s_account_find_common(id>>10, &id, sizeof(id), offset, account);
}

int am_account_find_tel(const char* tel, struct account_data_t* account)
{
    uint32_t fileid = 0;
    size_t offset = 0;
    if (strlen(tel) >= sizeof(account->data_basic.tel)) {
        fprintf(stderr, "account find error, the size of key is too large.\n");
        return 0;
    }

    offset = offsetof(struct account_basic_t, tel) + offsetof(struct account_data_t, data_basic);
    for (; fileid<ACCOUNT_FILE_NUM; ++fileid) {
        if (s_account_find_common(fileid, (void*)tel, strlen(tel) + 1, offset, account) == 1) {
            return 1;
        }
    }

    return 0;
}

int am_account_find_usernum(const char* usernum, struct account_data_t* account)
{
    uint32_t fileid = 0;
    size_t offset = 0;
    if (strlen(usernum) >= sizeof(account->data_basic.usernum)) {
        fprintf(stderr, "account find error, the size of key is too large.\n");
        return 0;
    }

    offset = offsetof(struct account_basic_t, usernum) + offsetof(struct account_data_t, data_basic);
    for (; fileid<ACCOUNT_FILE_NUM; ++fileid) {
        if (s_account_find_common(fileid, (void*)usernum, strlen(usernum) + 1, offset, account) == 1) {
            return 1;
        }
    }

    return 0;
}

int am_account_find_login(const char* login, struct account_data_t* account)
{
    size_t len_first = strlen(login);
    size_t len_second = strlen(login + len_first + 1); 
    uint32_t fileid = 0;

    for (; fileid < ACCOUNT_FILE_NUM; ++fileid) {
        jxmutex_lock(&s_mutex_file[fileid]);
        if (s_fpcfg[fileid] == 0) {
            continue;
        }
        fseek(s_fpcfg[fileid], 0, SEEK_SET);
        while (!feof(s_fpcfg[fileid])) {
            if (fread(account, sizeof(*account), 1, s_fpcfg[fileid]) == 1) {
                if ((strncmp(account->data_basic.tel, login, len_first + 1) == 0) ||
                            (strncmp(account->data_basic.usernum, login, len_first + 1) == 0)) {
                    jxmutex_unlock(&s_mutex_file[fileid]);
                    if (strncmp(account->passwd, login + len_first + 1, len_second + 1) == 0) {
                        return 1;
                    } else {
                        return 2;
                    }
                }
            } else if (ferror(s_fpcfg[fileid])) {
                fprintf(stderr, "file - %s, line - %d, read file error.\n", __FILE__, __LINE__);
            }
        }
        jxmutex_unlock(&s_mutex_file[fileid]);
    }
    return 0;
}

void am_account_config_init(void)
{
    int fileid = 0;
    int errcode = 0;
    static int s_inited = 0;

    if (s_inited) {
        return;
    }

    for (fileid=0; fileid<ACCOUNT_FILE_NUM; ++fileid) {
        jxsprintf(s_cfgfile[fileid], sizeof(s_cfgfile[fileid]), "%u", fileid);

        /** all files are opened first of all to avoid unexpected errors arising when server's running. */
        if (jxfopen(&s_fpcfg[fileid], s_cfgfile[fileid], "ab+") != 0) {
            errcode = 1;
            jxfatal_ext(&errcode, jxerr_exit, "open file error.\n");
        }

        s_mutex_file[fileid] = jxmutex_create();
    }

    jxclearlist_add(s_am_account_config_clear, NULL);
    s_inited = 1;
}

void s_am_account_config_clear(void* unused)
{
    uint32_t fileid = 0;
    (void)unused;

    for (fileid=0; fileid<ACCOUNT_FILE_NUM; ++fileid) {
        if (s_fpcfg[fileid] != NULL) {
            if (fclose(s_fpcfg[fileid]) != 0) {
                fprintf(stderr, "close file error.\n");
            }
        }

        jxmutex_destroy(&s_mutex_file[fileid]);
    }
}

int am_account_data2basic(const struct account_data_t* data, struct account_basic_t* basic)
{
    return jxmemcpy(basic, sizeof(*basic), &data->data_basic, sizeof(*basic));
}

int am_account_print(FILE* streamptr, const struct account_data_t* account)
{
    return fprintf(
                streamptr,
                "account info: id - %u, name - %s, passwd - %s, grade - %u.\n",
                account->data_basic.id,
                account->data_basic.username,
                account->passwd,
                account->data_basic.grade);
}

int am_account_write(const struct account_data_t* account)
{
    uint32_t fileid = account->data_basic.id >> 10;

    if (jxmutex_lock(&s_mutex_file[fileid])) {
        fprintf(stderr, "file- %s, line- %d, jxmutex_lock error.\n", __FILE__, __LINE__);
        return 1;
    }
    if (s_fpcfg[fileid] == 0) {
        fprintf(stderr, "file-%u is not open.\n", fileid);
    }
    if (fseek(s_fpcfg[fileid], 0, SEEK_END) != 0) {
        fprintf(stderr, "write new account error, call fseek fail.\n");
        jxmutex_unlock(&s_mutex_file[fileid]);
        return 1;
    }
    if (fwrite((void*)account, sizeof(struct account_data_t), 1, s_fpcfg[fileid]) != 1) {
        fprintf(stderr, "write new account error, fwrite fail.\n");
        jxmutex_unlock(&s_mutex_file[fileid]);
        return 1;
    }

    if (fflush(s_fpcfg[fileid])) {
        fprintf(stderr, "file - %s, line - %d: fflush error.", __FILE__, __LINE__);
    }
    jxmutex_unlock(&s_mutex_file[fileid]);

    return 0;
}

int am_account_update(const struct account_data_t* account)
{
    FILE* fptmp = NULL;
    int errcode = 0;
    struct account_data_t account_tmp;
    char tmpname[16];
    uint32_t fileid = account->data_basic.id >> 10;

    jxsprintf(tmpname, sizeof(tmpname), "t_%u", fileid);
    jxfopen(&fptmp, tmpname, "ab+");
    if (fwrite(account, sizeof(*account), 1, fptmp) != 1) {
        fprintf(stderr, "update account error, cannot write account data to tmp file.\n");
        fclose(fptmp);
        return 1;
    }

    jxmutex_lock(&s_mutex_file[fileid]);
    rewind(s_fpcfg[fileid]);
    while (!feof(s_fpcfg[fileid])) {
        if (fread(&account_tmp, sizeof(account_tmp), 1, s_fpcfg[fileid]) == 1) {
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
    fclose(s_fpcfg[fileid]);
    remove(s_cfgfile[fileid]);
    rename(tmpname, s_cfgfile[fileid]);

    if (jxfopen(&s_fpcfg[fileid], s_cfgfile[fileid], "ab+") != 0) {
        errcode = 1;
        jxfatal_ext(&errcode, jxerr_exit, "update file fatal error, open file error.\n");
    }

    jxmutex_unlock(&s_mutex_file[fileid]);

    return 0;
}
