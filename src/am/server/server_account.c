/**
 * @file server_account.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周四 2015-11-12 19:51:35 中国标准时间
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

int am_account_create_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    char* randcode = NULL;
    struct account_data_t account;

    randcode = (char*)malloc(sizeof(char) * g_len_randomcode);
    s_gen_randcode(g_len_randomcode, randcode, '0', '9');

    /**
     * @brief The format of inmsg here is: 
     * ----------------------------------------------------------------------------------
     * | struct csmsg_header | user id(uint32_t) | process id(int32_t) | tel(char*) ... |
     * ----------------------------------------------------------------------------------
     */
    if (s_create_account(inmsg + sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t), randcode, &account) != 0) {
        return 1;
    }
    s_add_login(&account, &((struct csmsg_header*)inmsg)->sockaddr, addrlen);

    am_config_write(&account);

    csprintf(oustmsg, *outmsglen, &account, sizeof(account));
    *outmsglen = sizeof(account);

    return 0;
}

int am_account_login_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return 1;
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
