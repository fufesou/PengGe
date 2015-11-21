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
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Fri 2015-11-20 23:53:19 (+0800)
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
#include    "../../common/clearlist.h"
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

extern char g_succeed;
extern char g_fail;
extern uint32_t g_len_randomcode;
extern uint32_t g_curmaxid;
extern uint32_t g_timeout_verification;
extern uint32_t g_max_account_tmp;


static int s_thread_exit = 0;
static csthread_t s_handle_thread;
static csmutex_t s_mutex_login;
static csmutex_t s_mutex_tmp;
static LIST_HEAD(s_list_login);
static LIST_HEAD(s_list_tmp);


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
 * @brief s_account_create
 * @param tel
 * @param randcode
 * @param account
 * @return
 */
static int s_account_create(const char* tel, const char* randcode, struct account_data_t* account);


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

    (void)unused;

	while (!s_thread_exit) {
		delnode = s_list_tmp.next;
		while (delnode != (&s_list_tmp)) {

			csmutex_lock(s_mutex_tmp);
            node_tmp = container_of(delnode, struct list_account_tmp_t, listnode);
			delnode = delnode->next;

			if (s_account_tmp_timeout(node_tmp)) {
				s_account_tmp_remove(node_tmp);
			}
			csmutex_unlock(s_mutex_tmp);
		}

		cssleep(10 * 60 * 1000);
	}

	return 0;
}

void s_account_tmp_add(const char* tel, const char* randcode)
{
    uint32_t randcodelen = strlen(randcode);
	struct list_account_tmp_t* tmp_new = (struct list_account_tmp_t*)malloc(sizeof(struct list_account_tmp_t));

	cs_memcpy(tmp_new->account_tmp.tel, sizeof(tmp_new->account_tmp.tel), tel, strlen(tel) + 1);

    tmp_new->account_tmp.randcode = (char*)malloc(randcodelen + 1);
    cs_memcpy(tmp_new->account_tmp.randcode, randcodelen + 1, randcode, randcodelen);
    tmp_new->account_tmp.randcode[randcodelen] = '\0';
    tmp_new->account_tmp.size_randcode = randcodelen + 1;

	cstimelong_cur(&tmp_new->account_tmp.time_created);

    list_add(&tmp_new->listnode, &s_list_tmp);
}

void s_account_tmp_clear(void)
{
    struct list_account_tmp_t* node_tmp = NULL;
    struct list_head* delnode = s_list_tmp.next;

	csmutex_lock(s_mutex_tmp);
    while (delnode != (&s_list_tmp)) {
        node_tmp = container_of(delnode, struct list_account_tmp_t, listnode);
        free(node_tmp->account_tmp.randcode);
        free(delnode);
        delnode = delnode->next;
    }

    s_list_tmp.prev = &s_list_tmp;
    s_list_tmp.next = &s_list_tmp;
	csmutex_unlock(s_mutex_tmp);
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

int s_account_create(const char* tel, const char* randcode, struct account_data_t* account)
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
 * @param inmsg The format of inmsg here is: \n
 * ------------------------------------------------\n
 * | tel(char*) | ... |                            \n
 * ------------------------------------------------\n
 *
 * @param outmsg The format of outmsg here is: \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ------------------------------------------------\n
 * | fail(char) | ... |                            \n
 * ------------------------------------------------\n
 *
 * @param data_verification unused.
 * @param len_verification unused.
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_create_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
    char* randcode = NULL;

	(void)data_verification;
	(void)len_verification;

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
 * @brief  am_account_verify_reply 
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
 */
int am_account_verify_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
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

	csmutex_lock(s_mutex_tmp);
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

    if (s_account_create(inmsg, node_tmp->account_tmp.randcode, &account) != 0) {
        msg_err = msg_err_create;
		ret_stat = 4;
		goto err;
    }
	s_account_tmp_remove(node_tmp);
	csmutex_unlock(s_mutex_tmp);

    csmutex_lock(s_mutex_login);
    am_account_data2basic(&account, &account_basic);
    outmsg[0] = g_succeed;
    cs_memcpy(outmsg + 1, *outmsglen - 1, &account_basic, sizeof(account_basic));
    am_login_add(&s_list_login, &account, data_verification, len_verification);
	*outmsglen = sizeof(account_basic) + 1;
    csmutex_unlock(s_mutex_login);

    return 0;

err:
	outmsg[0] = g_fail;
	cs_memcpy(outmsg + 1, *outmsglen - 1, msg_err, strlen(msg_err) + 1);
    *outmsglen = 1 + strlen(msg_err) + 1;
    csmutex_unlock(s_mutex_tmp);

    return ret_stat;
}

/**
 * @brief  am_account_login_reply 
 *
 * @param inmsg The format of inmsg here is:  \n
 * ----------------------------------------------------------------------------\n
 * | tel or username(char*) | passwd(char*) | ... |                            \n
 * ----------------------------------------------------------------------------\n
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
int am_account_logout_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
    int ret_stat = 0;
	const char* errmsg = NULL;
    const char* msg_account_not_found = "account is not loged in.";
    const char* msg_outmsg = "invalid outmsg argument.";
    const char* msg_verification = "account not valid host.";
    const char* msg_write_account = "cannot update current account to database.";
    struct account_login_t* account_login = NULL;
    uint32_t id = ntohl(*(uint32_t*)(inmsg));

    if (*outmsglen <= 2 || outmsg == NULL) {
        ret_stat = 3;
        errmsg = msg_outmsg;
        goto error;
    }

    if (csmutex_lock(s_mutex_login) != 0) {
        fprintf(stderr, "file- %s, line- %d, csmutex_lock error.\n", __FILE__, __LINE__);
    }
    if ((account_login = am_login_find(&s_list_login, id)) == NULL) {
        errmsg = msg_account_not_found;
        ret_stat = 1;
        goto error;
    }
    if (memcmp(data_verification, account_login->data_verification, len_verification) != 0) {
        errmsg = msg_verification;
        ret_stat = 2;
        goto error;
    }
	
    if (am_login_remove_account(account_login) != 0) {
        cs_memcpy(outmsg + 1, *outmsglen - 1, msg_write_account, strlen(msg_write_account) + 1);
        *outmsglen = 1 + strlen(msg_write_account) + 1;
    } else {
        *(outmsg + 1) = 0;
        *outmsglen = 1;
    }
    csmutex_unlock(s_mutex_login);
    *outmsg = g_succeed;

    return 0;

error:
    *outmsg = g_fail;
    cs_memcpy(outmsg + 1, *outmsglen - 1, errmsg, strlen(errmsg) + 1);
    *outmsglen = 1 + strlen(errmsg) + 1;
    csmutex_unlock(s_mutex_login);
    return ret_stat;
}

/**
 * @brief  am_account_changeusername_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is \n
 * ---------------------------------------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | old username(char*) | passwd(char*) | new username(char*) | ... |                                    \n
 * ---------------------------------------------------------------------------------------------------------------------------\n
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
int am_account_changeusername_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* passwd = NULL;
	const char* username_old = NULL;
	const char* username_new = NULL;
	struct account_data_t* account_data = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg));

	if (s_account_find(id, &account_data, &account_database, &errmsg) != 0) {
        csprintf(outmsg, *outmsglen, "%c%s wiht id: %d.", g_fail, errmsg, id);
		*outmsglen = strlen(outmsg);
		return 1;
	}

    username_old = inmsg + sizeof(uint32_t);
    passwd = strchr(username_old, '\0') + 1;
    username_new = strchr(passwd, '\0') + 1;

    if ((strncmp(username_old, account_data->username, strlen(account_data->username)) == 0) &&
                (strncmp(passwd, account_data->passwd, strlen(account_data->passwd)) == 0)) {
        cs_memcpy(account_data->username, sizeof(account_data->username), username_new, strlen(username_new));

		if (account_data == (&account_database)) {
			csmutex_lock(s_mutex_login);
            am_login_add(&s_list_login, account_data, data_verification, len_verification);
			csmutex_unlock(s_mutex_login);
            csprintf(outmsg, *outmsglen, "%c%s.", g_succeed, "account is not in login list.");
		} else {
            csprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
		}
    } else {
        csprintf(outmsg, *outmsglen, "%c%s.", g_fail, "username or passwd error.");
    }

    *outmsglen = strlen(outmsg) + 1;
    return 0;
}

/**
 * @brief  am_account_changepasswd_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is \n
 * ---------------------------------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | username(char*) | passwd(char*) | new passwd(char*) | ... |                                    \n
 * ---------------------------------------------------------------------------------------------------------------------\n
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

    username = inmsg + sizeof(uint32_t);
    passwd_old = strchr(username, '\0') + 1;
    passwd_new = strchr(passwd_old, '\0') + 1;

    if ((strncmp(username, account_login->username, strlen(account_login->username)) == 0) &&
                (strncmp(passwd_old, account_login->passwd, strlen(account_login->passwd)) == 0)) {
        cs_memcpy(account_login->passwd, sizeof(account_login->passwd), passwd_new, strlen(passwd_new));

		if (account_login == (&account_database)) {
			csmutex_lock(s_mutex_login);
            am_login_add(&s_list_login, account_login, data_verification, len_verification);
			csmutex_unlock(s_mutex_login);
            csprintf(outmsg, *outmsglen, "%c%s.", g_succeed, "account is not in login list.");
		} else {
            csprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
		}
    }  else {
        csprintf(outmsg, *outmsglen, "%c%s.", g_fail, "username or passwd error.");
    }

    *outmsglen = strlen(outmsg) + 1;
    return 0;
}

/**
 * @brief  am_account_changegrade_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is \n
 * -----------------------------------------------------------------------------------------------------------\n
 * | user id(uint32_t) | username(char*) | passwd(char*) | grade(uint32_t) | ... |                            \n
 * -----------------------------------------------------------------------------------------------------------\n
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
int am_account_changegrade_reply(char* inmsg, const void* data_verification, uint32_t len_verification, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* username = NULL;
	const char* passwd = NULL;
	struct account_data_t* account_login = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg));

	if (s_account_find(id, &account_login, &account_database, &errmsg) != 0) {
        csprintf(inmsg, *outmsglen, "%c%s wiht id: %d.", g_fail, errmsg, id);
		*outmsglen = strlen(outmsg);
		return 1;
	}

    username = inmsg + sizeof(uint32_t);
    passwd = strchr(username, '\0') + 1;

    if ((strncmp(username, account_login->username, strlen(account_login->username)) == 0) &&
                (strncmp(passwd, account_login->passwd, strlen(account_login->passwd)) == 0)) {
        account_login->grade = ntohl(*(uint32_t*)(strchr(passwd, '\0') + 1));

		if (account_login == (&account_database)) {
			csmutex_lock(s_mutex_login);
            am_login_add(&s_list_login, account_login, data_verification, len_verification);
			csmutex_unlock(s_mutex_login);
            csprintf(outmsg, *outmsglen, "%c%s.", g_succeed, "account is not in login list.");
		} else {
            csprintf(outmsg, *outmsglen, "%c%c", g_succeed, '\0');
		}
    } else {
        csprintf(outmsg, *outmsglen, "%c%s.", g_fail, "username or passwd error.");
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

    s_mutex_login = csmutex_create();
	s_mutex_tmp = csmutex_create();

    if (csthread_create(s_thread_clear_tmp, NULL, &s_handle_thread) != 0) {
		fprintf(stderr, "am_server: create tmp account cleaner thread error.\n");
        csmutex_destroy(s_mutex_login);
        csmutex_destroy(s_mutex_tmp);
		return 1;
	}

    am_account_config_init();
    csclearlist_add(s_am_server_account_clear, NULL);
    s_inited = 1;

    return 0;
}

void s_am_server_account_clear(void* unused)
{
	(void)unused;

	csmutex_lock(s_mutex_login);
    if (am_login_write(&s_list_login) != 0) {
		fprintf(stderr, "fatal error, current account data cannot be update to the database!\n");
	}
	am_login_clear(&s_list_login);
	csmutex_unlock(s_mutex_login);

    s_account_tmp_clear();
	s_thread_exit = 1;
    csthread_wait_terminate(s_handle_thread);

    csmutex_destroy(s_mutex_login);
    csmutex_destroy(s_mutex_tmp);
}
