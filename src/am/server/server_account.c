/**
 * @file server_account.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周五 2015-11-13 01:48:22 中国标准时间
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#endif

#include  <time.h>
#include  <malloc.h>
#include  <stdint.h>
#include  <stdlib.h>
#include    "list.h"
#include    "sock_types.h"
#include    "msgwrap.h"
#include    "../common/account_macros.h"
#include    "../common/account.h"
#include    "../common/account_config.h"

#pragma pack(4)
struct account_login_t {
    struct account_basic_t account_basic;

    struct sockaddr addr;
    cssocklen_t addrlen;
};

struct list_login_t {
    struct list_head listnode;
    struct account_login_t* account;
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

static LIST_HEAD(s_list_login);

static void s_gen_randcode(int len, char* code);

/**
 * @brief  s_create_account Initial user name will not be created.
 *
 * @param tel
 * @param randcode
 * @param addr
 * @param addrlen
 */
static int s_create_account(const char* tel, const char* randcode, struct sockaddr* addr, cssocklen_t addrlen);

static void s_send_randcode(const char* tel, const char* randcode);

static void s_add_login(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen);

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

/**
 * @brief  s_update_login This function will add new login account to login account list if previous same login have not happened.
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
static int s_update_login(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen);

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
    if (csprintf(account->tel, SIZEOF_ARR(account->tel), tel, strlen(len) + 1) != 0) {
        return 1;
    }
    memset(account->passwd, '\0', sizeof(account->passwd));
    return csprintf(account->passwd, SIZEOF_ARR(account->passwd), randcode, strlen(randcode) + 1);
}

void s_add_login(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen)
{
    struct list_login* login_node = NULL;

    login_node = (struct list_login_t*)malloc(sizeof(struct list_login_t));
    login_node->account = (struct account_login_t*)malloc(sizeof(struct account_login_t));
    login_node->account->grade = account->grade;
    login_node->account->id = account->id;
    csprintf(login_node->account->tel, sizeof(login_node->account->tel), account->tel, sizeof(account->tel));
    csprintf(&login_node->account->addr, sizeof(login_node->account->addr), &account->addr, account->addrlen);
    login_node->account->addrlen = account->addrlen;

    list_add(&login_node->listnode, &s_list_login);
}

int s_login_exist(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen)
{
    int login_status = 0;
    struct list_login_t* login_data = NULL;
    struct list_head* node_login = s_list_login.next;

    while (node_login != (&s_list_login)) {
        login_data = container_of(delnode, struct list_login_t, listnode);
        if (login_data->account_basic.id == account->id) {
            login_status = 1;

            if (memcpy(&login_data->addr, addr, addrlen) == 0) {
                login_status = 2;
                return login_status;
            }
        }
        node_login = node_login->next;
    }

    return login_status;
}

int s_update_login(const struct account_data_t* account, const struct sockaddr* addr, cssocklen_t addrlen)
{
    int login_status = s_login_eixt(account, addr, addrlen);
    if (login_status == 0) {
        s_add_login(account, addr, addrlen);
    }
    return login_status;
}

/**
 * @brief  am_account_create_reply This function handle the request of creating a new account
 *
 * @param inmsg The format of inmsg here is: 
 * ------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | tel(char*) | ... |
 * ------------------------------------------------------------------------------------
 *
 * @param inmsglen
 * @param outmsg The format of outmsg here is:
 * -------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | account(struct account_basic_t) | ... | 
 * -------------------------------------------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | fail(char) | ... |
 * ------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_create_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    static int fixedlen = sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t);
    char* randcode = NULL;
    struct account_data_t account;
    struct account_basic_t account_basic;

    randcode = (char*)malloc(sizeof(char) * g_len_randomcode);
    s_gen_randcode(g_len_randomcode, randcode, '0', '9');

    cs_memcpy(outmsg, *outmsglen, inmsg, fixedlen);

    if (s_create_account(inmsg + fixedlen, randcode, &account) != 0) {
        outmsg[fixedlen] = g_fail;
        ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1;
        *outmsglen = fixedlen + 1;
        return 1;
    }
    s_add_login(&account, &((struct csmsg_header*)inmsg)->sockaddr, addrlen);

    am_config_write(&account);

    outmsg[fixedlen] = g_succeed;
    ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1 + sizeof(account);

    am_account_data2basic(&account, &account_bacic);
    csprintf(oustmsg + fixedlen + 1, *outmsglen - fixedlen - 1, &account_baic, sizeof(account_basic));
    *outmsglen = sizeof(account_basic) + fixedlen + 1;

    return 0;
}

/**
 * @brief  am_account_login_reply 
 *
 * @param inmsg The format of inmsg here is: 
 * --------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | tel or username(char*) | passwd(char*) ... |
 * --------------------------------------------------------------------------------------------------------------
 *
 * @param inmsglen
 * @param outmsg
 * -------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | account(struct account_basic_t) | ... | 
 * -------------------------------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | succeed(char) | account(struct account_basic_t) | additional message | 
 * ----------------------------------------------------------------------------------------------------------------------------------------
 *  or
 * ----------------------------------------------------------------------------------------------------
 * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * ----------------------------------------------------------------------------------------------------
 *
 * @param outmsglen
 *
 * @return  0 if succeed, 1 if fail. 
 */
int am_account_login_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    static int fixedlen = sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t);
    int login_status = 0;
    struct account_data_t account;
    struct account_basic_t account_basic;
    char* login_begin = inmsg + len_before_login;
    const char* msg_account_not_exist = "account not exist.";
    const char* msg_account_login_fail = "tel(user name) or password error.";
    const char* msg_account_login_other = "this account have been logined on other socket.";

    cs_memcpy(outmsg, *outmsglen, inmsg, fixedlen);

    if (am_account_find_tel_username(login_begin, &account) != 0) {
        csprintf(outmsg + fixedlen, *outmsglen - fixedlen, "%c%s", g_fail, msg_account_not_exist);
        ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1 + strlen(msg_account_not_exist) + 1;
        *outmsglen = fixedlen + 1 + strlen(msg_account_not_exist) + 1;
        return 1;
    }
    if (strncmp(strchr(login_begin, '\0'), account.passwd, strlen(account.passwd) + 1) != 0) {
        csprintf(outmsg + fixedlen, *outmsglen - fixedlen, "%c%s", g_fail, msg_account_login_fail);
        ((struct csmsg_header*)outmsg)->numbytes = sizeof(uint32_t) + sizeof(int32_t) + 1 + strlen(msg_account_login_fail) + 1;
        *outmsglen = fixedlen + 1 + strlen(msg_account_login_fail) + 1;
        return 1;
    }

    am_account_data2basic(&account, &account_basic);
    outmsg[fixedlen] = g_succeed;
    csprintf(oustmsg + fixedlen + 1, *outmsglen - fixedlen - 1, &account_basic, sizeof(account_basic));

    login_status = s_update_login(&account, &((struct csmsg_header*)inmsg)->addr, ((struct csmsg_header*)inmsg)->addrlen);
    if (login_status == 1) {
        csprintf(outmsglen + fixedlen + 1 + sizeof(account_basic),
                *outmsglen - fixedlen - 1 - sizeof(account_basic),
                msg_account_login_other,
                strlen(msg_account_login_other) + 1);

        *outmsglen = sizeof(account_basic) + fixedlen + 1 + strlen(msg_account_login_other) + 1;
    } else {
        *outmsglen = sizeof(account_basic) + fixedlen + 1;
    }

    return 0;
}

int am_account_inquire_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return 1;
}

int am_account_changeusername_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return 1;
}

int am_account_changepasswd_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return 1;
}

int am_account_changegrade_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return 1;
}

int am_server_account_init(void)
{
}

int am_server_account_clear(void)
{
    struct list_login_t* login_node = NULL;
    struct list_head* delnode = s_list_login.next;

    while (delnode != (&s_list_login)) {
        login_node = container_of(delnode, struct list_login_t, listnode);
        free(delnode->account);
        free(delnode);
        delnode = delnode->next;
    }

    s_list_login.prev = &s_list_login;
    s_list_login.next = &s_list_login;
}
