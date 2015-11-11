/**
 * @file server_account.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  周三 2015-11-11 23:55:18 中国标准时间
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#endif

#include  <stdint.h>
#include  <stdlib.h>
#include    "list.h"
#include    "sock_types.h"
#include    "account_macros.h"
#include    "account.h"

#pragma pack(4)
struct account_login_t {
    struct account_basic_t account_basic;

    struct sockaddr addr;
    cssocklen_t addrlen;
};

struct list_login {
    struct list_head listnode;
    struct account_login_t account;
};

#pragma pack()

#ifdef __cplusplus
extern "C"
{
#endif

static LIST_HEAD(s_list_login);

static void s_add_login();

#ifdef __cplusplus
}
#endif

int am_account_create_reply(char* inmsg, int inmsglen, char* outmsg, __inout int* outmsglen)
{
    return 1;
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

