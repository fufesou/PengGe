/**
 * @file server_account.c
 * @brief  This file defines some basic account process functions for server. 
 *
 * Some message formats are described in the document: \n
 * ---------------------------------------------------------------------------\n
 * | data name(type) | (*) data name(type) | ... |                            \n
 * ---------------------------------------------------------------------------\n
 * - The data name is the name of this segment, such as 'username'.
 * - The type describe the segement's type, such as 'int8'.
 * - (*) means this segement is not filled with data by now.
 * - ... means unused remainder place.
 *
 * Server maintains a list of loged user account, and the verification of these accounts is the 'struct sockaddr'.
 *
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Sun 2015-12-06 18:06:30 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <netinet/in.h>
#endif

#ifndef _MSC_VER /* *nix */
#include  <semaphore.h>
#endif

#include  <time.h>
#include  <assert.h>
#include  <malloc.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/jxiot.h"
#include    "common/list.h"
#include    "common/timespan.h"
#include    "common/lightthread.h"
#include    "common/clearlist.h"
#include    "common/utility_wrap.h"
#include    "am/account_macros.h"
#include    "am/account.h"
#include    "am/account_file.h"
#include    "am/account_login.h"


struct account_tmp_t
{
    char usernum[ACCOUNT_USERNUM_LEN];
    char tel[ACCOUNT_TEL_LEN];
    char* randcode;
    uint8_t size_randcode;
    jxtimelong_t time_created;
};

struct list_account_tmp_t
{
    struct list_head listnode;
    struct account_tmp_t account_tmp;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern char g_succeed;
extern char g_fail;
extern uint32_t g_len_randomcode;
extern uint32_t g_curmaxid;
extern uint32_t g_timeout_verification;
extern uint32_t g_max_account_tmp;
extern char g_curmaxnum[ACCOUNT_USERNUM_LEN];


static int s_thread_exit = 0;
static jxthread_t s_handle_thread;
static jxmutex_t s_mutex_login;
static jxmutex_t s_mutex_tmp;
static LIST_HEAD(s_list_login);
static LIST_HEAD(s_list_tmp);

static const char* s_usernum_inc(char* num, size_t len);
static void s_gen_randcode(int len, char* code, char low, char high);
static void s_gen_randcode_test(int len, char* code, char low, char high);

static void s_am_server_account_clear(void*);

#ifdef WIN32
    static unsigned int __stdcall s_thread_clear_tmp(void*);
#else
    static void* s_thread_clear_tmp(void*);
#endif

/**
 * @warning These operations may be thread unsafe.
 */

/**
 * @brief  s_account_tmp_add s_account_tmp_add will add temporary account to be the
 * next of s_list_tmp, making the account sorted automatically by created time.
 *
 * @param info
 * @param randcode
 *
 * @note When applying other operations on the temporary account list, the sort attribute helps.
 *
 * @sa s_account_tmp_clear s_account_tmp_find s_account_tmp_timeout
 */
static void s_account_tmp_add(const char* info, const char* randcode);
static void s_account_tmp_clear(void);
static void s_account_tmp_remove(struct list_account_tmp_t* list_tmp);
static struct list_account_tmp_t* s_account_tmp_find(const char* tel);
static int s_account_tmp_timeout(struct list_account_tmp_t* list_tmp);


/**
 * @brief  s_account_created This function will try finding account by telphone number in both login list and database.
 *
 * @param tel
 *
 * @return  0 if not created, 1 if created. 
 */
static int s_account_created(const char* tel);

/**
 * @brief s_account_create
 * @param tel
 * @param account
 * @return
 */
static int s_account_create(struct account_tmp_t* account_tmp, struct account_data_t* account);


#ifdef __cplusplus
}
#endif


/**
 * @brief  s_thread_clear_tmp This function clear the temporary account list every 10 minites.
 *
 * @param unused
 *
 * @return  unused. 
 */
#ifdef WIN32
    unsigned int __stdcall s_thread_clear_tmp(void* unused)
#else
    void* s_thread_clear_tmp(void* unused)
#endif
{
    struct list_account_tmp_t* node_tmp = NULL;
    struct list_head* delnode = NULL;
    const uint32_t interval = 10 * 60;
    jxtimelong_t curtime;

    (void)unused;

    jxtimelong_cur(&curtime);

    while (!s_thread_exit) {
        if (jxtimelong_span_sec(&curtime) > interval) {
            jxtimelong_cur(&curtime);
        } else {
            continue;
        }

        delnode = s_list_tmp.next;
        while (delnode != (&s_list_tmp)) {

            jxmutex_lock(&s_mutex_tmp);
            node_tmp = container_of(delnode, struct list_account_tmp_t, listnode);
            delnode = delnode->next;

            if (s_account_tmp_timeout(node_tmp)) {
                s_account_tmp_remove(node_tmp);
            }
            jxmutex_unlock(&s_mutex_tmp);
        }

        // jxsleep(10 * 60 * 1000);
    }

    return 0;
}

const char* s_usernum_inc(char* num, size_t len)
{
    char* plast = strchr(num, '\0') - 1;
    char* pmove = plast;
    while (pmove != (num - 1)) {
        if (*pmove == '9') {
            *pmove-- = '0';
        } else {
            *pmove += 1;
            break;
        }
    }
    if (pmove == (num - 1)) {
        if ((size_t)(plast - num) > (len - 2)) {
            fprintf(stderr, "too many users registered.");
            return NULL;
        }
        *(plast + 2) = '\0';
        *(plast + 1) = '0';
    }
    return num;
}

void s_account_tmp_add(const char* info, const char* randcode)
{
    uint32_t randcodelen = strlen(randcode);
    struct list_account_tmp_t* tmp_new = (struct list_account_tmp_t*)calloc(1, sizeof(struct list_account_tmp_t));
    const char* usernum = info;
    char* tel = strchr(usernum, '\0') + 1;

    if (*usernum == '\0') {
        s_usernum_inc(g_curmaxnum, sizeof(g_curmaxnum));
        usernum = &g_curmaxnum[0];
    }
    jxmemcpy(tmp_new->account_tmp.usernum, sizeof(tmp_new->account_tmp.usernum), usernum, strlen(usernum) + 1);
    jxmemcpy(tmp_new->account_tmp.tel, sizeof(tmp_new->account_tmp.tel), tel, strlen(tel) + 1);

    tmp_new->account_tmp.randcode = (char*)calloc(randcodelen + 1, sizeof(char));
    jxmemcpy(tmp_new->account_tmp.randcode, randcodelen + 1, randcode, randcodelen);
    tmp_new->account_tmp.size_randcode = randcodelen + 1;

    jxtimelong_cur(&tmp_new->account_tmp.time_created);

    list_add(&tmp_new->listnode, &s_list_tmp);
}

void s_account_tmp_clear(void)
{
    struct list_account_tmp_t* node_tmp = NULL;
    struct list_head* delnode = s_list_tmp.next;

    jxmutex_lock(&s_mutex_tmp);
    while (delnode != (&s_list_tmp)) {
        node_tmp = container_of(delnode, struct list_account_tmp_t, listnode);
        delnode = delnode->next;
        s_account_tmp_remove(node_tmp);
    }

    s_list_tmp.prev = &s_list_tmp;
    s_list_tmp.next = &s_list_tmp;
    jxmutex_unlock(&s_mutex_tmp);
}

void s_account_tmp_remove(struct list_account_tmp_t* list_tmp)
{
    list_del(&list_tmp->listnode);
    free(list_tmp->account_tmp.randcode);
    free(list_tmp);
}

struct list_account_tmp_t* s_account_tmp_find(const char* tel)
{
    struct list_head* node_list = s_list_tmp.next;
    struct list_account_tmp_t* node_tmp = NULL;

    while (node_list != (&s_list_tmp)) {
        node_tmp = container_of(node_list, struct list_account_tmp_t, listnode);
        if (strncmp(node_tmp->account_tmp.tel, tel, strlen(tel)) == 0) {
            return node_tmp;
        }
    }

    return NULL;
}

int s_account_tmp_timeout(struct list_account_tmp_t* list_tmp)
{
    return (jxtimelong_span_sec(&list_tmp->account_tmp.time_created) > g_timeout_verification);
}


void s_gen_randcode(int len, char* code, char low, char high)
{
    int i = 0;
    int range = high - low;
    srand((uint32_t)time(NULL));

    assert(low < high && low > 0);
    while (i < len) {
        code[i++] = (rand() % range) + low;
    }
    code[len] = '\0';
}

void s_gen_randcode_test(int len, char* code, char low, char high)
{
    int i = 0;

    (void)low;
    (void)high;

    while (i < len) {
        code[i++] = '1';
    }
    code[len] = '\0';
}

int s_account_create(struct account_tmp_t* account_tmp, struct account_data_t* account)
{
    char rand_passwd[sizeof(account->passwd)];
    char rand_username[sizeof(account->data_basic.username)];

    size_t len_rand_passwd = 6;
    size_t len_rand_username = 8;

    account->data_basic.grade = 0;
    account->data_basic.id = (g_curmaxid++);
    if (jxmemcpy(
                account->data_basic.tel,
                SIZEOF_ARR(account->data_basic.tel),
                account_tmp->tel,
                strlen(account_tmp->tel) + 1) != 0) {
        return 1;
    }

    memset(account->data_basic.username, '\0', sizeof(account->data_basic.username));
    if (len_rand_username > (sizeof(rand_username) - 1)) {
        len_rand_username = sizeof(rand_username) - 1;
    }
    s_gen_randcode(len_rand_username, rand_username, 'a', 'z');
    if (jxmemcpy(
                    account->data_basic.username,
                    SIZEOF_ARR(account->data_basic.username),
                    rand_username,
                    strlen(rand_username) + 1) != 0) {
        return 1;
    }

    memset(account->passwd, '\0', sizeof(account->passwd));
    if (len_rand_passwd > (sizeof(rand_passwd) - 1)) {
        len_rand_passwd = sizeof(rand_passwd) - 1;
    }
#ifdef _DEBUG
    s_gen_randcode_test(len_rand_passwd, rand_passwd, 'a', 'z');
#else
    s_gen_randcode(len_rand_passwd, rand_passwd, 'a', 'z');
#endif
    jxmemcpy(
                account->data_basic.usernum,
                SIZEOF_ARR(account->data_basic.usernum),
                account_tmp->usernum,
                sizeof(account_tmp->usernum));
    return jxmemcpy(
                account->passwd,
                SIZEOF_ARR(account->passwd),
                rand_passwd,
                strlen(rand_passwd) + 1);
}

int s_account_created(const char* tel)
{
    struct account_data_t account;
    if (am_login_find_tel(&s_list_login, tel) != NULL) {
        return 1;
    }

    return am_account_find_tel(tel, &account);
}

/**
 * @brief  am_account_create_reply This function handle the request of creating a new account.
 *
 * One telphone number can only create one account. After a new account is created, the only
 * fields are telphone and random code. And the newly created account with created time will 
 * be added to a temporary list. 
 *
 * The temporary account list will clear the timeout temporary account periodically.
 *
 * @param inmsg The format of inmsg here is: \n
 * -----------------------------------------------------------------\n
 * | usernum(char*) | tel(char*) | ... |                            \n
 * -----------------------------------------------------------------\n
 *
 * @param outmsg The format of outmsg here is: \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param data_verification unused.
 * @param len_verification unused.
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_create_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen)
{
    char* randcode = NULL;
    char* tel = strchr(inmsg, '\0') + 1;
    const char* msg_exist = "This telephone has already been registered.";

    (void)data_verification;
    (void)len_verification;

    if (s_account_created(tel)) {
        jxsprintf(outmsg, *outmsglen, "%c%s", g_fail, msg_exist);
        *outmsglen = 1 + strlen(msg_exist) + 1;
        return 1;
    }

    randcode = (char*)malloc(g_len_randomcode + 1);

#ifdef _DEBUG
    s_gen_randcode_test(g_len_randomcode, randcode, '0', '9');
#else
    s_gen_randcode(g_len_randomcode, randcode, '0', '9');
#endif

    s_account_tmp_add(inmsg, randcode);

    outmsg[0] = g_succeed;
    *outmsglen = 1;

    return 0;
}

/**
 * @brief  am_account_verify_reply This function will verify the telephone and random code for newly created temporary account.
 *
 * If this verification passed, an actual account with random "username-passwd" will be added to the login list.
 *
 * @param inmsg The format of inmsg here is: \n
 * ------------------------------------------------------------------\n
 * | tel(char*) | randcode(char*) | ... |                            \n
 * ------------------------------------------------------------------\n
 *
 * @param data_verification
 * @param len_verification
 * @param outmsg The format of outmsg here is: \n
 * -------------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | ... |                            \n 
 * -------------------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return  
 *
 * @note Verifying mutliple times is allowed here.
 */
int am_account_verify_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen)
{
    struct account_data_t account;
    struct account_basic_t account_basic;
    struct list_account_tmp_t* node_tmp = NULL;
    const char* msg_err_tel = "telephone number is wrong.";
    const char* msg_err_randcode = "random code is wrong.";
    const char* msg_err_timeout = "timeout, please register again.";
    const char* msg_err_create = "create account fail with unkown error.";
    const char* msg_err = NULL;
    int ret_stat = 0;

    jxmutex_lock(&s_mutex_tmp);
    if ((node_tmp = s_account_tmp_find(inmsg)) == NULL) {
        msg_err = msg_err_tel;
        ret_stat = 1;
        goto err;
    }
    
    if (s_account_tmp_timeout(node_tmp)) {
        msg_err = msg_err_timeout;
        ret_stat = 2;
        goto err;
    }

    if (memcmp(inmsg + strlen(inmsg) + 1, node_tmp->account_tmp.randcode, strlen(node_tmp->account_tmp.randcode) + 1) != 0) {
        msg_err = msg_err_randcode;
        ret_stat = 3;
        goto err;
    }

    if (s_account_create(&node_tmp->account_tmp, &account) != 0) {
        msg_err = msg_err_create;
        ret_stat = 4;
        goto err;
    }
    s_account_tmp_remove(node_tmp);
    jxmutex_unlock(&s_mutex_tmp);

    jxmutex_lock(&s_mutex_login);
    am_account_data2basic(&account, &account_basic);
    account_basic.id = htonl(account_basic.id);   /**< id is converted to net endian */
    outmsg[0] = g_succeed;
    jxmemcpy(outmsg + 1, *outmsglen - 1, &account_basic, sizeof(account_basic));
    am_login_add(&s_list_login, &account, data_verification, len_verification);
    *outmsglen = sizeof(account_basic) + 1;
    jxmutex_unlock(&s_mutex_login);

    return 0;

err:
    outmsg[0] = g_fail;
    jxmemcpy(outmsg + 1, *outmsglen - 1, msg_err, strlen(msg_err) + 1);
    *outmsglen = 1 + strlen(msg_err) + 1;
    jxmutex_unlock(&s_mutex_tmp);

    return ret_stat;
}

/**
 * @brief  am_account_login_reply 
 *
 * @param inmsg The format of inmsg here is:  \n
 * ----------------------------------------------------------------\n
 * | tel(char*) | passwd(char*) | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg The format of outmsg is \n
 * -------------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | ... |                            \n 
 * -------------------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | additional message | ... |                            \n
 * ----------------------------------------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 *
 * @warning Consider of efficience, this function dose not prevent the same host from loging in the same account.
 * This may lead to heavy list if the user maliciously continually log in the same account.
 */
int am_account_login_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen)
{
    int login_status = 0;
    struct account_data_t account;
    struct account_basic_t account_basic;
    const char* msg_account_not_exist = "account not exist.";
    const char* msg_account_passwd = "invalid keywords or passwd";
    const char* msg_account_login_other = "this account have been logined on other socket.";

    login_status = am_account_find_login(inmsg, &account);
    if (login_status == 0) {
        jxsprintf(outmsg, *outmsglen, "%c%s", g_fail, msg_account_not_exist);
        *outmsglen = 1 + strlen(msg_account_not_exist);
        return 1;
    } else if (login_status == 2) {
        jxsprintf(outmsg, *outmsglen, "%c%s", g_fail, msg_account_passwd);
        *outmsglen = 1 + strlen(msg_account_passwd);
        return 1;
    }

    am_account_data2basic(&account, &account_basic);
    account_basic.id = htonl(account_basic.id);
    outmsg[0] = g_succeed;
    jxmemcpy(outmsg + 1, *outmsglen - 1, &account_basic, sizeof(account_basic));

    jxmutex_lock(&s_mutex_login);
    login_status = am_login_tryadd(&s_list_login, &account, data_verification, len_verification);
    jxmutex_unlock(&s_mutex_login);
    if (login_status == 1) {
        jxmemcpy(outmsg + 1 + sizeof(account_basic),
                *outmsglen - 1 - sizeof(account_basic),
                msg_account_login_other,
                strlen(msg_account_login_other) + 1);

        *outmsglen = sizeof(account_basic) + 1 + strlen(msg_account_login_other) + 1;
    } else {
        outmsg[sizeof(account_basic) + 1] = '\0';
        *outmsglen = sizeof(account_basic) + 1;
    }

    return 0;
}

/**
 * @brief  am_account_logout_reply 
 *
 * @param inmsg The format of inmsg is \n
 * -------------------------------------------------------\n
 * | user id(uint32_t) | ... |                            \n
 * -------------------------------------------------------\n
 *
 * @param data_verification
 * @param len_verification
 * @param outmsg The format of outmsg is \n
 * -----------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | ... |                            \n 
 * -----------------------------------------------------------------------\n
 *  or \n
 * --------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | additional message | ... |                            \n
 * --------------------------------------------------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | fail(char) | error message | ... |                            \n
 * ------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_logout_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen)
{
    const char* msg_account_not_found = "account is not loged in.";
    const char* msg_outmsg = "invalid outmsg argument.";
    const char* msg_verification = "account not valid host.";
    const char* msg_write_account = "cannot update current account to database.";
    struct account_login_t* account_login = NULL;
    uint32_t id = ntohl(*(uint32_t*)(inmsg));

    if (*outmsglen <= 2 || outmsg == NULL) {
        *outmsg = g_fail;
        jxmemcpy(outmsg + 1, *outmsglen - 1, msg_outmsg, strlen(msg_outmsg) + 1);
        *outmsglen = 1 + strlen(msg_outmsg) + 1;
        return 3;
    }

    if (jxmutex_lock(&s_mutex_login) != 0) {
        fprintf(stderr, "file- %s, line- %d, jxmutex_lock error.\n", __FILE__, __LINE__);
    }
    if ((account_login = am_login_find_id(&s_list_login, id)) == NULL) {
        *outmsg = g_fail;
        jxmemcpy(outmsg + 1, *outmsglen - 1, msg_account_not_found, strlen(msg_account_not_found) + 1);
        *outmsglen = 1 + strlen(msg_account_not_found) + 1;
        jxmutex_unlock(&s_mutex_login);
        return 1;
    }
    if (memcmp(data_verification, account_login->data_verification, len_verification) != 0) {
        *outmsg = g_fail;
        jxmemcpy(outmsg + 1, *outmsglen - 1, msg_verification, strlen(msg_verification) + 1);
        *outmsglen = 1 + strlen(msg_verification) + 1;
        jxmutex_unlock(&s_mutex_login);
        return 2;
    }
    
    if (am_login_remove_account(account_login) != 0) {
        jxmemcpy(outmsg + 1, *outmsglen - 1, msg_write_account, strlen(msg_write_account) + 1);
        *outmsglen = 1 + strlen(msg_write_account) + 1;
    } else {
        *(outmsg + 1) = 0;
        *outmsglen = 1;
    }
    jxmutex_unlock(&s_mutex_login);
    *outmsg = g_succeed;

    return 0;
}

/**
 * @brief  am_account_changeusername_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is \n
 * -----------------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | passwd(char*) | new username(char*) | ... |                                    \n
 * -----------------------------------------------------------------------------------------------------\n
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg The format of outmsg is \n
 * -----------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | ... |                            \n 
 * -----------------------------------------------------------------------\n
 *  or \n
 * --------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | additional message | ... |                            \n
 * --------------------------------------------------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | fail(char) | error message | ... |                            \n
 * ------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changeusername_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen)
{
    const char* passwd = NULL;
    const char* username_new = NULL;
    struct account_data_t* account_data = NULL;
    uint32_t id = ntohl(*(uint32_t*)(inmsg));

    jxmutex_lock(&s_mutex_login);
    account_data = &am_login_find_id_verification(&s_list_login, id, data_verification, len_verification)->account;
    jxmutex_unlock(&s_mutex_login);

    if (account_data == NULL) {
        jxsprintf(outmsg, *outmsglen, "%c%s wiht id: %d.", g_fail, "account is not logged in.", id);
        *outmsglen = strlen(outmsg);
        return 1;
    }

    passwd = inmsg + sizeof(uint32_t);
    username_new = strchr(passwd, '\0') + 1;

    if (strncmp(passwd, account_data->passwd, strlen(account_data->passwd)) == 0) {
        jxmemcpy(
                    account_data->data_basic.username,
                    sizeof(account_data->data_basic.username),
                    username_new,
                    strlen(username_new) + 1);
        jxsprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
    } else {
        jxsprintf(outmsg, *outmsglen, "%c%s.", g_fail, "passwd error.");
    }

    *outmsglen = strlen(outmsg) + 1;
    return 0;
}

/**
 * @brief  am_account_changepasswd_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is \n
 * ---------------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | passwd(char*) | new passwd(char*) | ... |                                    \n
 * ---------------------------------------------------------------------------------------------------\n
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg The format of outmsg is \n
 * -----------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | ... |                            \n 
 * -----------------------------------------------------------------------\n
 *  or \n
 * --------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | additional message | ... |                            \n
 * --------------------------------------------------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | fail(char) | error message | ... |                            \n
 * ------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changepasswd_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen)
{
    const char* passwd_old = NULL;
    const char* passwd_new = NULL;
    struct account_data_t* account_login = NULL;
    uint32_t id = ntohl(*(uint32_t*)(inmsg));

    jxmutex_lock(&s_mutex_login);
    account_login = &am_login_find_id_verification(&s_list_login, id, data_verification, len_verification)->account;
    jxmutex_unlock(&s_mutex_login);

    if (account_login == NULL) {
        jxsprintf(outmsg, *outmsglen, "%c%s wiht id: %d.", g_fail, "account is not logged in.", id);
        *outmsglen = strlen(outmsg);
        return 1;
    }

    passwd_old = inmsg + sizeof(uint32_t);
    passwd_new = strchr(passwd_old, '\0') + 1;

    if (strncmp(passwd_old, account_login->passwd, strlen(account_login->passwd)) == 0) {
        jxmemcpy(account_login->passwd, sizeof(account_login->passwd), passwd_new, strlen(passwd_new) + 1);
        jxsprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
    }  else {
        jxsprintf(outmsg, *outmsglen, "%c%s.", g_fail, "passwd error.");
    }

    *outmsglen = strlen(outmsg) + 1;
    return 0;
}

/**
 * @brief  am_account_changegrade_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is \n
 * -----------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | passwd(char*) | grade(uint32_t) | ... |                            \n
 * -----------------------------------------------------------------------------------------\n
 *
 * @param outmsg The format of outmsg is \n
 * -----------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | ... |                            \n 
 * -----------------------------------------------------------------------\n
 *  or \n
 * --------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | succeed(char) | additional message | ... |                            \n
 * --------------------------------------------------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | fail(char) | error message | ... |                            \n
 * ------------------------------------------------------------------------------------\n
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_changegrade_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __jxinout uint32_t* outmsglen)
{
    const char* passwd = NULL;
    struct account_data_t* account_login = NULL;
    uint32_t id = ntohl(*(uint32_t*)(inmsg));

    jxmutex_lock(&s_mutex_login);
    account_login = &am_login_find_id_verification(&s_list_login, id, data_verification, len_verification)->account;
    jxmutex_unlock(&s_mutex_login);

    if (account_login == NULL) {
        jxsprintf(outmsg, *outmsglen, "%c%s wiht id: %d.", g_fail, "account is not logged in.", id);
        *outmsglen = strlen(outmsg);
        return 1;
    }

    passwd = inmsg + sizeof(uint32_t);

    if (strncmp(passwd, account_login->passwd, strlen(account_login->passwd)) == 0) {
        account_login->data_basic.grade = (uint8_t)ntohl(*(uint32_t*)(strchr(passwd, '\0') + 1));
        jxsprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
    } else {
        jxsprintf(outmsg, *outmsglen, "%c%s.", g_fail, "passwd error.");
    }

    *outmsglen = strlen(outmsg) + 1;
    return 0;
}

int am_server_account_init(void)
{
    static int s_inited = 0;
    if (s_inited) {
        return 0;
    }

    s_mutex_login = jxmutex_create();
    s_mutex_tmp = jxmutex_create();

    if (jxthread_create(s_thread_clear_tmp, NULL, &s_handle_thread) != 0) {
        fprintf(stderr, "am_server: create tmp account cleaner thread error.\n");
        jxmutex_destroy(&s_mutex_login);
        jxmutex_destroy(&s_mutex_tmp);
        return 1;
    }

    am_account_config_init();
    jxclearlist_add(s_am_server_account_clear, NULL);
    s_inited = 1;

    return 0;
}

void s_am_server_account_clear(void* unused)
{
    (void)unused;

    jxmutex_lock(&s_mutex_login);
    if (am_login_write(&s_list_login) != 0) {
        fprintf(stderr, "fatal error, current account data cannot be update to the database!\n");
    }
    am_login_clear(&s_list_login);
    jxmutex_unlock(&s_mutex_login);

    s_account_tmp_clear();
    s_thread_exit = 1;
    jxthread_wait_terminate(s_handle_thread);

    jxmutex_destroy(&s_mutex_login);
    jxmutex_destroy(&s_mutex_tmp);
}
