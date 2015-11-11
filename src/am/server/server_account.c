/**
 * @file server_account.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周三 2015-11-11 19:28:56 中国标准时间
 */

#include    "list.h"
#include    "account.h"

#pragma pack(4)
struct account_login_t {
    struct account_basic_t account_basic;

    struct sockaddr addr;
    cssocklen_t addrlen;
};

struct list_login {
    struct list_heade listnode;
    struct account_login_t account;
};

#pragma pack()

#ifdef __cplusplus
extern "C"
{
#endif

static LIST_HEAD(s_list_login)

static void s_add_login();

#ifdef __cplusplus
}
#endif

int am_account_create_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_login_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_inquire_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_changeusername_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_changepasswd_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

int am_account_changegrade_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
}

