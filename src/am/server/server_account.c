/**
 * @file server_account.c
 * @brief  This file defines some basic account process functions for server. 
 * Some message formats are described in the document:
 * -----------------------------------------------
 * | data name(type) | (*) data name(type) | ... |
 * -----------------------------------------------
 * 1. The data name is the name of this segment, such as 'username'.
 * 2. The type describe the segement's type, such as 'int8'.
 * 3. (*) means this segement is not filled with data by now.
 * 4. ... means unused remainder place.
 *
 * Server maintains a list of loged user account, and the verification of these accounts is the 'struct sockaddr'.
 *
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Tue 2015-11-17 00:23:40 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <netinet/in.h>
#endif

#include  <time.h>
#include  <assert.h>
#include  <malloc.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <semaphore.h>
#include    "../../common/config_macros.h"
#include    "../../common/macros.h"
#include    "../../common/list.h"
#include    "../../common/timespan.h"
#include    "../../common/lightthread.h"
#include    "../../common/utility_wrap.h"
#include    "../common/account_macros.h"
#include    "../common/account.h"
#include    "../common/account_file.h"
#include    "account_login.h"


struct account_tmp_t
{
	char tel[ACCOUNT_TEL_LEN];
	char* randcode;
	uint8_t size_randcode;
	cstimelong_t time_created;
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

extern int g_len_randomcode;
extern char g_succeed;
extern char g_fail;
extern uint32_t g_curmaxid;
extern uint32_t g_timeout_verification;
extern uint32_t g_max_account_tmp;


static csmutex_t s_mutex_login;
static csmutex_t s_mutex_tmp;
static LIST_HEAD(s_list_login);
static LIST_HEAD(s_list_tmp);


static void s_gen_randcode(int len, char* code, char low, char high);
static void s_send_randcode(const char* tel, const char* randcode);


#ifdef WIN32
	static unsigned int __stdcall s_account_tmp_clear(void*);
#else
	static void* s_account_tmp_clear(void*);
#endif

static void s_account_tmp_add(const char* tel, const char* randcode);
static void s_account_tmp_clear(void);
static void s_account_tmp_remove(struct list_account_tmp_t* list_tmp);
static struct list_account_tmp_t* s_account_tmp_find(const char* tel);
static int s_account_tmp_timeout(struct list_account_tmp_t* list_tmp);

/**
 * @brief  s_account_find Find account with 'id' in login list or in the database.
 *
 * @param id
 * @param account_find The pointer to the find result.
 * @param account_database The pointer to the database find result.
 * @param errmsg The pointer to the error message.
 *
 * @return   0 if the account with 'id' is found.
 */
static int s_account_find(uint32_t id, struct account_data_t** account_find, struct account_data_t* account_database, const char** errmsg);

/**
 * @brief s_create_account
 * @param tel
 * @param randcode
 * @param account
 * @return
 */
static int s_create_account(const char* tel, const char* randcode, struct account_data_t* account);


#ifdef __cplusplus
}
#endif

#ifdef WIN32
	unsigned int __stdcall s_account_tmp_clear(void* unused)
#else
	void* s_account_tmp_clear(void* unused)
#endif
{
}

void s_account_tmp_add(const char* tel, const char* randcode)
{
	struct list_account_tmp_t* tmp_new = (struct list_account_tmp_t*)malloc(sizeof(struct list_account_tmp_t));

	cs_memcpy(tmp_new->account_tmp.tel, sizeof(tmp_new->account_tmp.tel), tel, strlen(tel) + 1);

	tmp_new->account_tmp.randcode = (char*)malloc(strlen(randcode) + 1);
	cs_memcpy(tmp_new->account_tmp.randcode, strlen(randcode) + 1, randcode, sizeof(randcode) + 1);
	tmp_new->account_tmp.size_randcode = strlen(randcode) + 1;

	cstimelong_cur(&tmp_new->account_tmp.time_created);

	list_add(&tmp_new.listnode, s_list_tmp);
}

void s_account_tmp_clear(void)
{
    struct list_account_tmp_t* node_tmp = NULL;
    struct list_head* delnode = s_list_tmp.next;

    while (delnode != (&s_list_tmp)) {
        node_tmp = container_of(delnode, struct list_login_t, listnode);
        free(node_tmp->account_tmp.randcode);
        free(delnode);
        delnode = delnode->next;
    }

    s_list_tmp.prev = &s_list_tmp;
    s_list_tmp.next = &s_list_tmp;
}

void s_account_tmp_remove(struct list_account_tmp_t* list_tmp)
{
	list_del(&list_tmp->listnode);
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
	return (cstimelong_span_sec(&list_tmp->account_tmp.time_created) > g_timeout_verification);
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
}

int s_create_account(const char* tel, const char* randcode, struct account_data_t* account)
{
    account->grade = 0;
    account->id = (g_curmaxid++);
    if (cs_memcpy(account->tel, SIZEOF_ARR(account->tel), tel, strlen(tel) + 1) != 0) {
        return 1;
    }
    memset(account->passwd, '\0', sizeof(account->passwd));
    return cs_memcpy(account->passwd, SIZEOF_ARR(account->passwd), randcode, strlen(randcode) + 1);
}

int s_account_find(uint32_t id, struct account_data_t** account_find, struct account_data_t* account_database, const char** errmsg)
{
	const char* msg_not_found = "cannot find account.";
	const char* msg_unknown_err = "unkown error occurs, login account does not appear in login msg";

	*errmsg = NULL;

	csmutex_lock(s_mutex_login);
	*account_find = &am_login_find(&s_list_login, id)->account;
	csmutex_unlock(s_mutex_login);
    if ((*account_find) == NULL) {
        if (am_account_find_id(id, account_database) != 0) {
			*errmsg = msg_not_found;
			return 1;
		} else {
			*account_find = account_database;
			*errmsg = msg_unknown_err;
		}
	} 

	return 0;
}

/**
 * @brief  am_account_create_reply This function handle the request of creating a new account
 *
 * @param inmsg The format of inmsg here is: 
 * --------------------
 * | tel(char*) | ... |
 * --------------------
 *
 * @param outmsg The format of outmsg here is:
 * -----------------------
 * | succeed(char) | ... |
 * -----------------------
 *  or
 * --------------------
 * | fail(char) | ... |
 * --------------------
 *
 * @param data_verification
 * @param len_verification
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_create_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
    char* randcode = NULL;
    struct account_data_t account;

    randcode = (char*)malloc(sizeof(char) * g_len_randomcode);
    s_gen_randcode(g_len_randomcode, randcode, '0', '9');

    if (s_create_account(inmsg, randcode, &account) != 0) {
        outmsg[0] = g_fail;
        *outmsglen = 1;
        return 1;
    }

	csmutex_lock(s_mutex_login);
    am_login_add(&s_list_login, &account, data_verification, len_verification);
	csmutex_unlock(s_mutex_login);

    am_account_write(&account);

    outmsg[0] = g_succeed;
    *outmsglen = 1;

    return 0;
}

/**
 * @brief  am_account_login_reply 
 *
 * @param inmsg The format of inmsg here is: 
 * ------------------------------------------------
 * | tel or username(char*) | passwd(char*) | ... |
 * ------------------------------------------------
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg
 * -------------------------------------------------------------------------------
 * | succeed(char) | account(struct account_basic_t) | ... | 
 * -------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------
 * | succeed(char) | account(struct account_basic_t) | additional message | ... |
 * ----------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------
 * | fail(char) | error message | ... |
 * ----------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_login_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
    int login_status = 0;
    struct account_data_t account;
    struct account_basic_t account_basic;
    const char* msg_account_not_exist = "account not exist.";
    const char* msg_account_login_fail = "tel(user name) or password error.";
    const char* msg_account_login_other = "this account have been logined on other socket.";

    if (am_account_find_tel_username(inmsg, &account) != 0) {
        csprintf(outmsg, *outmsglen, "%c%s", g_fail, msg_account_not_exist);
        *outmsglen = 1 + strlen(msg_account_not_exist);
        return 1;
    }
    if (strncmp(strchr(inmsg, '\0'), account.passwd, strlen(account.passwd) + 1) != 0) {
        csprintf(outmsg, *outmsglen, "%c%s", g_fail, msg_account_login_fail);
        *outmsglen = 1 + strlen(msg_account_login_fail) + 1;
        return 1;
    }

    am_account_data2basic(&account, &account_basic);
    outmsg[0] = g_succeed;
    cs_memcpy(outmsg + 1, *outmsglen - 1, &account_basic, sizeof(account_basic));

	csmutex_lock(s_mutex_login);
    login_status = am_login_tryadd(&s_list_login, &account, data_verification, len_verification);
	csmutex_unlock(s_mutex_login);
    if (login_status == 1) {
        cs_memcpy(outmsg + 1 + sizeof(account_basic),
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
 * @brief  am_account_changeusername_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is
 * -------------------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | old username(char*) | passwd(char*) | new username(char*) | ... |
 * -------------------------------------------------------------------------------------------------------------
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg The format of outmsg is
 * -----------------------------------------------------------------
 * | user id(uint32_t) | succeed(char) | ... | 
 * -----------------------------------------------------------------
 *  or
 * --------------------------------------------------------------------------------------
 * | user id(uint32_t) | succeed(char) | additional message | ... |
 * --------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------
 * | user id(uint32_t) | fail(char) | error message | ... |
 * ------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changeusername_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* passwd = NULL;
	const char* username_old = NULL;
	const char* username_new = NULL;
	struct account_data_t* account_login = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg));

	if (s_account_find(id, &account_login, &account_database, &errmsg) != 0) {
		csprintf(inmsg, *outmsglen, "%c%s wiht id: %d.\n", g_fail, errmsg, id);
		*outmsglen = strlen(outmsg);
		return 1;
	}

	username_old = inmsg;
    passwd = strchr(username_old, '\0') + 1;
    username_new = strchr(passwd, '\0') + 1;

	if ((strncmp(username_old, account_login->username, strlen(account_login->username) + 1) == 0) && 
				(strncmp(passwd, account_login->passwd, strlen(account_login->passwd) + 1) == 0)) {
        cs_memcpy(account_login->username, sizeof(account_login->username), username_new, strlen(username_new) + 1);

		if (account_login == (&account_database)) {
			csmutex_lock(s_mutex_login);
            am_login_add(&s_list_login, account_login, data_verification, len_verification);
			csmutex_unlock(s_mutex_login);
            csprintf(outmsg, *outmsglen, "%c%s.\n", g_succeed, errmsg);
		} else {
            csprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
		}
        *outmsglen = strlen(outmsg) + 1;
    }

    return 0;
}

/**
 * @brief  am_account_changepasswd_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is
 * -----------------------------------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | username(char*) | passwd(char*) | new passwd(char*) | ... |
 * -----------------------------------------------------------------------------------------------------------------------------
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg The format of outmsg is
 * ---------------------------------------------------------------------------------------
 * | user id(uint32_t) | succeed(char) | ... | 
 * ---------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | succeed(char) | additional message | ... |
 * ------------------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | fail(char) | error message | ... |
 * ----------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changepasswd_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* username = NULL;
	const char* passwd_old = NULL;
	const char* passwd_new = NULL;
	struct account_data_t* account_login = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg));

	if (s_account_find(id, &account_login, &account_database, &errmsg) != 0) {
		csprintf(inmsg, *outmsglen, "%c%s wiht id: %d.\n", g_fail, errmsg, id);
		*outmsglen = strlen(outmsg);
		return 1;
	}

	username = inmsg;
    passwd_old = strchr(username, '\0') + 1;
    passwd_new = strchr(passwd_old, '\0') + 1;

	if ((strncmp(username, account_login->username, strlen(account_login->username) + 1) == 0) && 
				(strncmp(passwd_old, account_login->passwd, strlen(account_login->passwd) + 1) == 0)) {
        cs_memcpy(account_login->passwd, sizeof(account_login->passwd), passwd_new, strlen(passwd_new) + 1);

		if (account_login == (&account_database)) {
			csmutex_lock(s_mutex_login);
            am_login_add(&s_list_login, account_login, data_verification, len_verification);
			csmutex_unlock(s_mutex_login);
            csprintf(outmsg, *outmsglen, "%c%s.\n", g_succeed, errmsg);
		} else {
            csprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
		}
        *outmsglen = strlen(outmsg);
    }

    return 0;
}

/**
 * @brief  am_account_changegrade_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is
 * -----------------------------------------------------------------------------------------------------
 * | user id(uint32_t) | username(char*) | passwd(char*) | grade(uint32_t) | ... |
 * -----------------------------------------------------------------------------------------------------
 *
 * @param outmsg The format of outmsg is
 * -----------------------------------------------------------------
 * | user id(uint32_t) | succeed(char) | ... | 
 * -----------------------------------------------------------------
 *  or
 * --------------------------------------------------------------------------------------
 * | user id(uint32_t) | succeed(char) | additional message | ... |
 * --------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------
 * | user id(uint32_t) | fail(char) | error message | ... |
 * ------------------------------------------------------------------------------
 *
 * @param data_verification
 * @param len_verification
 *
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_changegrade_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* username = NULL;
	const char* passwd = NULL;
	struct account_data_t* account_login = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg));

	if (s_account_find(id, &account_login, &account_database, &errmsg) != 0) {
		csprintf(inmsg, *outmsglen, "%c%s wiht id: %d.\n", g_fail, errmsg, id);
		*outmsglen = strlen(outmsg);
		return 1;
	}

	username = inmsg;
    passwd = strchr(username, '\0') + 1;

	if ((strncmp(username, account_login->username, strlen(account_login->username) + 1) == 0) && 
				(strncmp(passwd, account_login->passwd, strlen(account_login->passwd) + 1) == 0)) {
        account_login->grade = ntohl(*(uint32_t*)(strchr(passwd, '\0') + 1));

		if (account_login == (&account_database)) {
			csmutex_lock(s_mutex_login);
            am_login_add(&s_list_login, account_login, data_verification, len_verification);
			csmutex_unlock(s_mutex_login);
            csprintf(outmsg, *outmsglen, "%c%s.\n", g_succeed, errmsg);
		} else {
            csprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
		}
        *outmsglen = strlen(outmsg);
    }

    return 0;
}

int am_server_account_init(void)
{
    s_mutex_login = csmutex_create();
	s_mutex_tmp = csmutex_create();
    return 0;
}

int am_server_account_clear(void)
{
	int ret = 1;

	csmutex_lock(s_mutex_login);
    if ((ret = am_login_write(&s_list_login)) != 0) {
		fprintf(stderr, "fatal error, current account data cannot be update to the database!\n");
	}
	am_login_clear(&s_list_login);
	csmutex_unlock(s_mutex_login);

	csmutex_destory(s_mutex_login);
	csmutex_destory(s_mutex_tmp);

    return ret;
}
