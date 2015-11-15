/**
 * @file server_account.c
 * @brief  This file defines some basic account process functions for server. 
 * * Some message formats are described in the document:
 * -----------------------------------------------
 * | data name(type) | (*) data name(type) | ... |
 * -----------------------------------------------
 * 1. The data name is the name of this segment, such as 'username'.
 * 2. The type describe the segement's type, such as 'int8'.
 * 3. (*) means this segement is not filled with data by now.
 * 4. ... means unused remainder place.

 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Sun 2015-11-15 16:47:22 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#endif

#include  <time.h>
#include  <assert.h>
#include  <malloc.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <config_macros.h>
#include    "macros.h"
#include    "list.h"
#include    "sock_types.h"
#include    "msgwrap.h"
#include    "utility_wrap.h"
#include    "../common/account_macros.h"
#include    "../common/account.h"
#include    "../common/account_file.h"

#pragma pack(4)
struct account_login_t {
	struct account_data_t account;

    struct sockaddr addr;
    cssocklen_t addrlen;
};

struct list_login_t {
    struct list_head listnode;
    struct account_login_t* account_sock;
};

#pragma pack()

#ifdef __cplusplus
extern "C"
{
#endif

extern int g_len_randomcode;
extern char g_succeed;
extern char g_fail;
extern uint32_t g_curmaxid;


static uint32_t s_headerlen = sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t);

static LIST_HEAD(s_list_login);

static void s_gen_randcode(int len, char* code, char low, char high);
static void s_send_randcode(const char* tel, const char* randcode);

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


/**************************************************
 **              the login block                 **
 **************************************************/
static void s_login_clear(void);
static void s_login_add(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen);

/**
 * @brief  s_login_tryadd This function will add new login account to login account list if previous same login have not happened.
 *
 * @param account
 * @param addr
 * @param addrlen
 *
 * @return
 * 1. 0 if no login of this account happened.
 * 2. 1 if login of the account happened on other socket.
 * 3. 2 if login  happened on the same socket.
 */
static int s_login_tryadd(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen);

static struct account_login_t* s_login_find(uint32_t id);

/**
 * @brief  s_login_exist 
 *
 * @param account
 * @param addr
 * @param addrlen
 *
 * @return   
 * 1. 0 if no login of this account happened.
 * 2. 1 if login of the account happened on other socket.
 * 3. 2 if login  happened on the same socket.
 */
static int s_login_exist(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen);

static int s_login_write(void);

#ifdef __cplusplus
}
#endif

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

    if (((*account_find) = (&s_login_find(id)->account)) == NULL) {
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


/**************************************************
 **              the login block                 **
 **************************************************/
void s_login_add(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen)
{
    struct list_login_t* login_node = NULL;

    login_node = (struct list_login_t*)malloc(sizeof(struct list_login_t));
    login_node->account_sock = (struct account_login_t*)malloc(sizeof(struct account_login_t));
	cs_memcpy(&login_node->account_sock->account, sizeof(login_node->account_sock->account), account, sizeof(*account));
    cs_memcpy(&login_node->account_sock->addr, sizeof(login_node->account_sock->addr), addr, addrlen);
    login_node->account_sock->addrlen = addrlen;

    list_add(&login_node->listnode, &s_list_login);
}

void s_login_clear(void)
{
    struct list_login_t* login_node = NULL;
    struct list_head* delnode = s_list_login.next;

    while (delnode != (&s_list_login)) {
        login_node = container_of(delnode, struct list_login_t, listnode);
        free(login_node->account_sock);
        free(delnode);
        delnode = delnode->next;
    }

    s_list_login.prev = &s_list_login;
    s_list_login.next = &s_list_login;
}

struct account_login_t* s_login_find(uint32_t id)
{
    struct list_login_t* login_data = NULL;
    struct list_head* node_login = s_list_login.next;

    while (node_login != (&s_list_login)) {
        login_data = container_of(node_login, struct list_login_t, listnode);
        if (login_data->account_sock->account.id == id) {
            return login_data->account_sock;
        }
        node_login = node_login->next;
    }

	return NULL;
}

int s_login_exist(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen)
{
    int login_status = 0;
    struct list_login_t* login_data = NULL;
    struct list_head* node_login = s_list_login.next;

    while (node_login != (&s_list_login)) {
        login_data = container_of(node_login, struct list_login_t, listnode);
        if (login_data->account_sock->account.id == account->id) {
            login_status = 1;

            if (memcpy(&login_data->account_sock->addr, addr, addrlen) == 0) {
                login_status = 2;
                return login_status;
            }
        }
        node_login = node_login->next;
    }

    return login_status;
}

int s_login_write(void)
{
    struct list_login_t* login_data = NULL;
    struct list_head* node_login = s_list_login.next;

    while (node_login != (&s_list_login)) {
        login_data = container_of(node_login, struct list_login_t, listnode);
        if (am_account_write(&login_data->account_sock->account) != 0) {
			return 1;
		}

        node_login = node_login->next;
    }

	return 0;
}

int s_login_tryadd(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen)
{
    int login_status = s_login_exist(account, addr, addrlen);
    if (login_status == 0) {
        s_login_add(account, addr, addrlen);
    }
    return login_status;
}


/**
 * @brief  am_account_create_reply This function handle the request of creating a new account
 *
 * @param inmsg The format of inmsg here is: 
 * ----------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | process id(int32_t) | tel(char*) | ... |
 * ----------------------------------------------------------------------------------------
 *
 * @param outmsg The format of outmsg here is:
 * ---------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | ... |
 * ---------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | fail(char) | ... |
 * ------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_create_reply(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    char* randcode = NULL;
    struct account_data_t account;

    randcode = (char*)malloc(sizeof(char) * g_len_randomcode);
    s_gen_randcode(g_len_randomcode, randcode, '0', '9');

    cs_memcpy(outmsg, *outmsglen, inmsg, s_headerlen);

    if (s_create_account(inmsg + s_headerlen, randcode, &account) != 0) {
        outmsg[s_headerlen] = g_fail;
        ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1;
        *outmsglen = s_headerlen + 1;
        return 1;
    }
    s_login_add(&account, &GET_HEADER_DATA(inmsg, addr), GET_HEADER_DATA(inmsg, addrlen));

    am_account_write(&account);

    outmsg[s_headerlen] = g_succeed;
    ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1 + sizeof(account);
    *outmsglen = s_headerlen + 1;

    return 0;
}

/**
 * @brief  am_account_login_reply 
 *
 * @param inmsg The format of inmsg here is: 
 * --------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | process id(int32_t) | tel or username(char*) | passwd(char*) | ... |
 * --------------------------------------------------------------------------------------------------------------------
 *
 * @param outmsg
 * -------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | account(struct account_basic_t) | ... | 
 * -------------------------------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | account(struct account_basic_t) | additional message | ... |
 * ----------------------------------------------------------------------------------------------------------------------------------------------
 *  or
 * --------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * --------------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_login_reply(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    int login_status = 0;
    struct account_data_t account;
    struct account_basic_t account_basic;
    char* login_begin = inmsg + s_headerlen;
    const char* msg_account_not_exist = "account not exist.";
    const char* msg_account_login_fail = "tel(user name) or password error.";
    const char* msg_account_login_other = "this account have been logined on other socket.";

    cs_memcpy(outmsg, *outmsglen, inmsg, s_headerlen);

    if (am_account_find_tel_username(login_begin, &account) != 0) {
        csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s", g_fail, msg_account_not_exist);
        ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1 + strlen(msg_account_not_exist) + 1;
        *outmsglen = s_headerlen + 1 + strlen(msg_account_not_exist);
        return 1;
    }
    if (strncmp(strchr(login_begin, '\0'), account.passwd, strlen(account.passwd) + 1) != 0) {
        csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s", g_fail, msg_account_login_fail);
        ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1 + strlen(msg_account_login_fail) + 1;
        *outmsglen = s_headerlen + 1 + strlen(msg_account_login_fail);
        return 1;
    }

    am_account_data2basic(&account, &account_basic);
    outmsg[s_headerlen] = g_succeed;
    cs_memcpy(outmsg + s_headerlen + 1, *outmsglen - s_headerlen - 1, &account_basic, sizeof(account_basic));

    login_status = s_login_tryadd(&account, &((struct csmsg_header*)inmsg)->addr, ((struct csmsg_header*)inmsg)->addrlen);
    if (login_status == 1) {
        cs_memcpy(outmsg + s_headerlen + 1 + sizeof(account_basic),
                *outmsglen - s_headerlen - 1 - sizeof(account_basic),
                msg_account_login_other,
                strlen(msg_account_login_other) + 1);

        *outmsglen = sizeof(account_basic) + s_headerlen + 1 + strlen(msg_account_login_other) + 1;
    } else {
        outmsg[sizeof(account_basic) + s_headerlen + 1] = '\0';
        *outmsglen = sizeof(account_basic) + s_headerlen;
    }

    return 0;
}

/**
 * @brief  am_account_changeusername_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is
 * -----------------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | old username(char*) | passwd(char*) | new username(char*) | ... |
 * -----------------------------------------------------------------------------------------------------------------------------------
 *
 * @param outmsg The format of outmsg is
 * ---------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | ... | 
 * ---------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | additional message | ... |
 * ------------------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * ----------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changeusername_reply(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* passwd = NULL;
	const char* username_old = NULL;
	const char* username_new = NULL;
	struct account_data_t* account_login = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));

    cs_memcpy(outmsg, *outmsglen, inmsg, s_headerlen);

	if (s_account_find(id, &account_login, &account_database, &errmsg) != 0) {
		csprintf(inmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s wiht id: %d.\n", g_fail, errmsg, id);
		*outmsglen = s_headerlen + strlen(outmsg + s_headerlen);
		return 1;
	}

	username_old = inmsg + s_headerlen;
    passwd = strchr(username_old, '\0') + 1;
    username_new = strchr(passwd, '\0') + 1;

	if ((strncmp(username_old, account_login->username, strlen(account_login->username) + 1) == 0) && 
				(strncmp(passwd, account_login->passwd, strlen(account_login->passwd) + 1) == 0)) {
        cs_memcpy(account_login->username, sizeof(account_login->username), username_new, strlen(username_new) + 1);

		if (account_login == (&account_database)) {
            s_login_add(account_login, &GET_HEADER_DATA(inmsg, addr), GET_HEADER_DATA(inmsg, addrlen));
            csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s.\n", g_succeed, errmsg);
		} else {
            csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%c", g_succeed, '\0');
		}
        *outmsglen = s_headerlen + strlen(outmsg + s_headerlen);
    }

    return 0;
}

/**
 * @brief  am_account_changepasswd_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is
 * -----------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | username(char*) | passwd(char*) | new passwd(char*) | ... |
 * -----------------------------------------------------------------------------------------------------------------------------
 *
 * @param outmsg The format of outmsg is
 * ---------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | ... | 
 * ---------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | additional message | ... |
 * ------------------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * ----------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return   
 */
int am_account_changepasswd_reply(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* username = NULL;
	const char* passwd_old = NULL;
	const char* passwd_new = NULL;
	struct account_data_t* account_login = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));

    cs_memcpy(outmsg, *outmsglen, inmsg, s_headerlen);

	if (s_account_find(id, &account_login, &account_database, &errmsg) != 0) {
		csprintf(inmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s wiht id: %d.\n", g_fail, errmsg, id);
		*outmsglen = s_headerlen + strlen(outmsg + s_headerlen);
		return 1;
	}

	username = inmsg + s_headerlen;
    passwd_old = strchr(username, '\0') + 1;
    passwd_new = strchr(passwd_old, '\0') + 1;

	if ((strncmp(username, account_login->username, strlen(account_login->username) + 1) == 0) && 
				(strncmp(passwd_old, account_login->passwd, strlen(account_login->passwd) + 1) == 0)) {
        cs_memcpy(account_login->passwd, sizeof(account_login->passwd), passwd_new, strlen(passwd_new) + 1);

		if (account_login == (&account_database)) {
            s_login_add(account_login, &GET_HEADER_DATA(inmsg, addr), GET_HEADER_DATA(inmsg, addrlen));
            csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s.\n", g_succeed, errmsg);
		} else {
            csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%c", g_succeed, '\0');
		}
        *outmsglen = s_headerlen + strlen(outmsg + s_headerlen);
    }

    return 0;
}

/**
 * @brief  am_account_changegrade_reply The client has already hold the new and old information. So, server only need to tell the client yes or no.
 *
 * @param inmsg The format of inmsg is
 * ---------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | username(char*) | passwd(char*) | grade(uint32_t) | ... |
 * ---------------------------------------------------------------------------------------------------------------------------
 *
 * @param outmsg The format of outmsg is
 * ---------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | ... | 
 * ---------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | additional message | ... |
 * ------------------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * ----------------------------------------------------------------------------------------------------
 *
 * @param inmsg
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 */
int am_account_changegrade_reply(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
	const char* errmsg = NULL;
	const char* username = NULL;
	const char* passwd = NULL;
	struct account_data_t* account_login = NULL;
	struct account_data_t account_database;
	uint32_t id = ntohl(*(uint32_t*)(inmsg + sizeof(struct csmsg_header)));

    cs_memcpy(outmsg, *outmsglen, inmsg, s_headerlen);

	if (s_account_find(id, &account_login, &account_database, &errmsg) != 0) {
		csprintf(inmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s wiht id: %d.\n", g_fail, errmsg, id);
		*outmsglen = s_headerlen + strlen(outmsg + s_headerlen);
		return 1;
	}

	username = inmsg + s_headerlen;
    passwd = strchr(username, '\0') + 1;

	if ((strncmp(username, account_login->username, strlen(account_login->username) + 1) == 0) && 
				(strncmp(passwd, account_login->passwd, strlen(account_login->passwd) + 1) == 0)) {
        account_login->grade = ntohl(*(uint32_t*)(strchr(passwd, '\0') + 1));

		if (account_login == (&account_database)) {
            s_login_add(account_login, &GET_HEADER_DATA(inmsg, addr), GET_HEADER_DATA(inmsg, addrlen));
            csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%s.\n", g_succeed, errmsg);
		} else {
            csprintf(outmsg + s_headerlen, *outmsglen - s_headerlen, "%c%c", g_succeed, '\0');
		}
        *outmsglen = s_headerlen + strlen(outmsg + s_headerlen);
    }

    return 0;
}

int am_server_account_init(void)
{
    return 0;
}

int am_server_account_clear(void)
{
	int ret = 1;
    if ((ret = s_login_write()) != 0) {
		fprintf(stderr, "fatal error, current account data cannot be update to the database!\n");
	}
	s_login_clear();
    return ret;
}
