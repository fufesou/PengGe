/**
 * @file client_account.c
 * @brief  This file defines some basic account process functions for client. 
 *
 * Some message formats are described in the document:\n
 * ---------------------------------------------------------------------------\n
 * | data name(type) | (*) data name(type) | ... |                            \n
 * ---------------------------------------------------------------------------\n
 * - The data name is the name of this segment, such as 'username'.
 * - The type describe the segement's type, such as 'int8'.
 * - (*) means this segement is not filled with data by now.
 * - ... means unused remainder place.
 *  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Fri 2015-12-25 01:26:47 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <netinet/in.h>
#endif

#include  <assert.h>
#include  <stdio.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/config_macros.h"
#include    "am/account_macros.h"
#include    "common/utility_wrap.h"
#include    "am/account.h"


#ifdef __cplusplus
extern "C" {
#endif

extern char g_succeed;
extern char g_fail;

static struct account_client_t
{
    struct account_basic_t account_basic;
    char passwd[ACCOUNT_PASSWD_LEN];
} s_account_client, s_account_tmp;


#ifdef __cplusplus
}
#endif


/**************************************************
 **             the request block                 **
 **************************************************/
int am_account_create_request(const char* usernum, const char* tel, char* outmsg, uint32_t* outmsglen)
{
    uint32_t len_num = 0;
    uint32_t len_tel = 0;
    uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
        return 1;
    }
    *(uint32_t*)outmsg = htonl(am_method_getid("account_create"));

    len_num = strlen(usernum);
    len_tel = strlen(tel);
    msglen = sizeof(uint32_t) + len_num + len_tel + 2;

    if (*outmsglen < msglen || len_tel == 0) {
        return -1;
    }
    if (jxmemcpy(outmsg + sizeof(uint32_t), *outmsglen - sizeof(uint32_t), usernum, len_num + 1) != 0) {
        return 1;
    }
    if (jxmemcpy(outmsg + sizeof(uint32_t) + len_num + 1, *outmsglen - sizeof(uint32_t) - len_num - 1, tel, len_tel + 1) != 0) {
        return 1;
    }

    *outmsglen = msglen;
    return 0;
}

int am_account_verify_request(const char* tel, const char* randcode, char* outmsg, uint32_t* outmsglen)
{
    uint32_t len_tel = 0;
    uint32_t len_randcode = 0;
    uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
        return 1;
    }
    *(uint32_t*)outmsg = htonl(am_method_getid("account_verify"));

    len_tel = strlen(tel);
    len_randcode = strlen(randcode);
    msglen = sizeof(uint32_t) + len_tel + len_randcode + 2;

    if (*outmsglen < msglen) {
        return 1;
    }
    if (jxmemcpy(outmsg + sizeof(uint32_t), *outmsglen - sizeof(uint32_t), tel, len_tel + 1) != 0) {
        return 1;
    }
    if (jxmemcpy(outmsg + sizeof(uint32_t) + len_tel + 1, *outmsglen - sizeof(uint32_t) - len_randcode - 1, randcode, len_randcode + 1) != 0) {
        return 1;
    }

    *outmsglen = msglen;
    return 0;
}

int am_account_login_request(const char* keywords, const char* passwd, char* outmsg, uint32_t* outmsglen)
{   
    uint32_t len_keywords = 0;
    uint32_t len_passwd = 0;
    uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
        return 1;
    }
    *(uint32_t*)outmsg = htonl(am_method_getid("account_login"));

    len_keywords = strlen(keywords);
    len_passwd = strlen(passwd);
    msglen = sizeof(uint32_t) + len_keywords + len_passwd + 2;

    if (*outmsglen < msglen) {
        return 1;
    }

    if (jxmemcpy(outmsg + sizeof(uint32_t), *outmsglen - sizeof(uint32_t), keywords, len_keywords + 1) != 0) {
        return 1;
    }
    if (jxmemcpy(outmsg + sizeof(uint32_t) + len_keywords + 1, *outmsglen - sizeof(uint32_t) - len_keywords - 1, passwd, len_passwd + 1) != 0) {
        return 1;
    }
    *outmsglen = msglen;
    jxmemcpy(s_account_client.passwd, sizeof(s_account_client.passwd), passwd, len_passwd + 1);

    return 0;
}

int am_account_logout_request(char* outmsg, uint32_t* outmsglen)
{
    if (*outmsglen < (sizeof(uint32_t) * 2)) {
        return 1;
    }
    *(uint32_t*)outmsg = htonl(am_method_getid("account_logout"));
    *(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(s_account_client.account_basic.id);
    *outmsglen= sizeof(uint32_t) * 2;
    return 0;
}

int am_account_changeusername_request(
            const char* passwd,
            const char* username_new,
            char* outmsg,
            uint32_t* outmsglen)
{
    uint32_t len_username_new = 0;
    uint32_t len_passwd = 0;
    uint32_t msglen = 0;

    if (*outmsglen < (sizeof(uint32_t) * 2)) {
        return 1;
    }
    *(uint32_t*)outmsg = htonl(am_method_getid("account_changeusername"));
    *(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(s_account_client.account_basic.id);

    len_username_new = strlen(username_new);
    len_passwd = strlen(passwd);
    msglen = sizeof(uint32_t) * 2 + len_passwd + len_username_new + 2;

    if (*outmsglen < msglen) {
        return 1;
    }

    if (jxmemcpy(outmsg + sizeof(uint32_t) * 2, *outmsglen - sizeof(uint32_t) * 2, passwd, len_passwd + 1) != 0) {
        return 1;
    }
    if (jxmemcpy(outmsg + sizeof(uint32_t) * 2 + len_passwd + 1, *outmsglen - sizeof(uint32_t) * 2 - len_passwd - 1, username_new, len_username_new + 1) != 0) {
        return 1;
    }
    *outmsglen = msglen;

    jxmemcpy(s_account_tmp.account_basic.username, sizeof(s_account_tmp.account_basic.username), username_new, strlen(username_new) + 1);

    return 0;
}

int am_account_changepasswd_request(
            const char* passwd_old,
            const char* passwd_new,
            char* outmsg,
            uint32_t* outmsglen)
{
    uint32_t len_passwd_old = 0;
    uint32_t len_passwd_new = 0;
    uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
        return 1;
    }
    *(uint32_t*)outmsg = htonl(am_method_getid("account_changepasswd"));
    *(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(s_account_client.account_basic.id);

    len_passwd_old = strlen(passwd_old);
    len_passwd_new = strlen(passwd_new);
    msglen = sizeof(uint32_t) * 2 + len_passwd_old + len_passwd_new + 2;

    if (*outmsglen < msglen) {
        return 1;
    }

    if (jxmemcpy(outmsg + sizeof(uint32_t) * 2, *outmsglen - sizeof(uint32_t) * 2, passwd_old, len_passwd_old + 1) != 0) {
        return 1;
    }
    if (jxmemcpy(
                    outmsg + sizeof(uint32_t) * 2 + len_passwd_old + 1,
                    *outmsglen - sizeof(uint32_t) * 2 - len_passwd_old - 1,
                    passwd_new,
                    len_passwd_new + 1) != 0) {
        return 1;
    }
    *outmsglen = msglen;

    jxmemcpy(s_account_tmp.passwd, sizeof(s_account_tmp.passwd), passwd_new, strlen(passwd_new) + 1);

    return 0;
}

int am_account_changegrade_request(
            const char* passwd,
            uint8_t grade,
            char* outmsg,
            uint32_t* outmsglen)
{
    uint32_t len_passwd = 0;
    uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
        return 1;
    }
    *(uint32_t*)outmsg = htonl(am_method_getid("account_changepasswd"));
    *(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(s_account_client.account_basic.id);

    len_passwd = strlen(passwd);
    msglen = sizeof(uint32_t) * 2 + len_passwd + 1 + sizeof(grade);
    if (*outmsglen < msglen) {
        return 1;
    }

    if (jxmemcpy(outmsg + sizeof(uint32_t) * 2, *outmsglen - sizeof(uint32_t) * 2, passwd, len_passwd + 1) != 0) {
        return 1;
    }
    *(uint8_t*)(outmsg + sizeof(uint32_t) * 2 + len_passwd + 1) = grade;
    *outmsglen = msglen;

    s_account_tmp.account_basic.grade = grade;

    return 0;
}

#define TO4C(a) *(char*)&a, ((char*)&a)[1], ((char*)&a)[2], ((char*)&a)[3]

/**************************************************
 **              the react block                 **
 **************************************************/
/**
 * @brief  am_account_create_react This is the first message returned from server. And this message is just a notification message.
 *
 * @param inmsg The format of inmsg is: \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg the format of outmsg is: \n
 * ------------------------------------------------------------------------------------------------\n
 * | "account_create"(char*) | msglen(uint32_t) | succeed(char) | ... |                            \n
 * ------------------------------------------------------------------------------------------------\n
 *  or \n
 * -------------------------------------------------------------------------------------------------------------\n
 * | "account_create"(char*) | msglen(uint32_t) | fail(char) | error message | ... |                            \n
 * -------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen unused.
 *
 * @return   The return value has no meaning, and it always is 0.
 */
int am_account_create_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    uint32_t len_result = 0;
    if (inmsg[0] == g_succeed) {
        printf("client: create account succeed, please wait for a moment to receiving the random telcode, then revify it.\n");
        if (outmsg != NULL) {
            len_result = 1;
            jxsprintf(outmsg, *outmsglen, "account_create%c%c%c%c%c", TO4C(len_result), g_succeed);
        }
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: account_create fail, %s\n", inmsg + 1);
        if (outmsg != NULL) {
            len_result = 1 + (int)strlen(inmsg + 1) + 1;
            jxsprintf(outmsg, *outmsglen, "account_create%c%c%c%c%c%s", TO4C(len_result), g_fail, inmsg + 1);
        }
        return 1;
    } else {
        fprintf(stderr, "client: account_create - unkown message.\n");
        return -2;
    }

    return 0;
}

/**
 * @brief  am_account_verify_react 
 *
 * @param inmsg The format of outmsg here is: \n
 * ---------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | ... |                        \n
 * ---------------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | additional message | ... |                            \n
 * ----------------------------------------------------------------------------------------------------------\n

 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg the format of outmsg is: \n
 * -------------------------------------------------------------------------------------------------------------\n
 * | "account_verify"(char*) | msglen(uint32_t) | succeed(char) | account(struct account_basic_t) | ... |       \n
 * -------------------------------------------------------------------------------------------------------------\n
 *  or \n                                                        
 * ---------------------------------------------------------------------------------------------------------------------------------\n
 * | "account_login"(char*) | msglen(uint32_t) | succeed(char) | account(struct account_basic_t) | additional message | ... |       \n
 * ---------------------------------------------------------------------------------------------------------------------------------\n
 *  or \n
 * -------------------------------------------------------------------------------------------------------------\n
 * | "account_verify"(char*) | msglen(uint32_t) | fail(char) | error message | ... |                            \n
 * -------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see jxmemcpy for return code.
 *
 * @sa jxmemcpy
 */
int am_account_verify_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    int ret = 1;
    uint32_t len_result = 0;
    const char* sig_msg = "account_verify";
    size_t len_sig = strlen(sig_msg);
    size_t size_account = sizeof(struct account_basic_t);

    if (inmsg[0] == g_succeed) {
        if ((ret = (jxmemcpy(&s_account_client.account_basic, sizeof(struct account_basic_t), inmsg + 1, sizeof(struct account_basic_t)))) != 0) {
            fprintf(stderr, "client: account_verify suceed, but client cannot update account data.\n");
            return ret;
        }

        s_account_client.account_basic.id = ntohl(s_account_client.account_basic.id);
        fprintf(stdout, "client: account_verify succeed.\n");

        if (inmsg[1 + sizeof(struct account_basic_t)] != 0) {
            fprintf(stdout, ", additional message from server - %s.\n", inmsg + 1 + sizeof(struct account_basic_t));

            if (outmsg != NULL) {
                len_result = 1 + size_account + (int)strlen(inmsg + 1 + size_account) + 1;
                assert(*outmsglen > (len_sig + len_result));
                jxmemcpy(
                            outmsg + len_sig + sizeof(uint32_t) + size_account + 1,
                            *outmsglen - len_sig - sizeof(uint32_t) - size_account - 1,
                            inmsg + 1 + size_account,
                            strlen(inmsg + 1 + size_account) + 1);
            }
        } else if (outmsg != NULL){
            len_result = 1 + size_account;
            assert(*outmsglen > (len_sig + len_result));
            jxmemcpy(outmsg, *outmsglen, sig_msg, len_sig);
            *((uint32_t*)(outmsg + len_sig)) = len_result;
            *(outmsg + len_sig + sizeof(uint32_t)) = g_succeed;
            jxmemcpy(outmsg + len_sig + sizeof(uint32_t) + 1, *outmsglen - len_sig - sizeof(uint32_t) - 1, inmsg + 1, size_account);
        }
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: %s fail, %s\n", sig_msg, inmsg + 1);
        if (outmsg != NULL) {
            len_result = 1 + (int)strlen(inmsg + 1) + 1;
            jxsprintf(outmsg, *outmsglen, "%s%c%c%c%c%c%s", sig_msg, TO4C(len_result), g_fail, inmsg + 1);
        }
        return 1;
    } else {
        fprintf(stderr, "client: %s - unkown message.\n", sig_msg);
        return -2;
    }

    return ret;
}

/**
 * @brief  am_account_login_react This function estimate whether the login succeed. 
 *
 * @param inmsg The format of inmsg is: \n
 * ---------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | ... |                        \n
 * ---------------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | additional message | ... |                            \n
 * ----------------------------------------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg the format of outmsg is: \n
 * ------------------------------------------------------------------------------------------------------------\n
 * | "account_login"(char*) | msglen(uint32_t) | succeed(char) | account(struct account_basic_t) | ... |       \n
 * ------------------------------------------------------------------------------------------------------------\n
 *  or \n                                                        
 * ---------------------------------------------------------------------------------------------------------------------------------\n
 * | "account_login"(char*) | msglen(uint32_t) | succeed(char) | account(struct account_basic_t) | additional message | ... |       \n
 * ---------------------------------------------------------------------------------------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------------------------------------------\n
 * | "account_login"(char*) | msglen(uint32_t) | fail(char) | error message | ... |                            \n
 * ------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return
 * - 0 if succeed.
 * - 1 if fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see jxmemcpy for return code.
 *
 * @sa jxmemcpy
 */
int am_account_login_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    int ret = 1;
    uint32_t len_result = 0;
    const char* sig_msg = "account_login";
    size_t len_sig = strlen(sig_msg);
    size_t size_account = sizeof(struct account_basic_t);

    if (inmsg[0] == g_succeed) {
        if ((ret = (jxmemcpy(&s_account_client.account_basic, sizeof(struct account_basic_t), inmsg + 1, size_account))) != 0) {
            fprintf(stderr, "client: account_loginsuceed, but client cannot update account data.\n");
            return ret;
        }

        s_account_client.account_basic.id = ntohl(s_account_client.account_basic.id);
        fprintf(stdout, "client: login succeed.\n");

        if (inmsg[1 + sizeof(struct account_basic_t)] != 0) {
            fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1 + size_account);

            if (outmsg != NULL) {
                len_result = 1 + size_account + (int)strlen(inmsg + 1 + size_account) + 1;
                assert(*outmsglen > (len_sig + len_result));
                jxmemcpy(
                            outmsg + len_sig + sizeof(uint32_t) + size_account + 1,
                            *outmsglen - len_sig - sizeof(uint32_t) - size_account - 1,
                            inmsg + 1 + size_account,
                            strlen(inmsg + 1 + size_account) + 1);
            }
        } else if (outmsg != NULL){
            len_result = 1 + size_account;
            assert(*outmsglen > (len_sig + len_result));
            jxmemcpy(outmsg, *outmsglen, sig_msg, len_sig);
            *((uint32_t*)(outmsg + len_sig)) = len_result;
            *(outmsg + len_sig + sizeof(uint32_t)) = g_succeed;
            jxmemcpy(outmsg + len_sig + sizeof(uint32_t) + 1, *outmsglen - len_sig - sizeof(uint32_t) - 1, inmsg + 1, size_account);
        }
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: %s fail, %s\n", sig_msg, inmsg + 1);
        if (outmsg != NULL) {
            len_result = 1 + (int)strlen(inmsg + 1) + 1;
            jxsprintf(outmsg, *outmsglen, "%s%c%c%c%c%c%s", sig_msg, TO4C(len_result), g_fail, inmsg + 1);
        }
        return 1;
    } else {
        fprintf(stderr, "client: %s - unkown message.\n", sig_msg);
        return -2;
    }

    return ret;
}

/**
 * @brief  am_account_logout_react 
 *
 * @param inmsg The format of inmsg is \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------\n
 * | succeed(char) | additional message | ... |                            \n
 * ------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg the format of outmsg is: \n
 * ------------------------------------------------------------------------------------------------\n
 * | "account_logout"(char*) | msglen(uint32_t) | succeed(char) | ... |                            \n
 * ------------------------------------------------------------------------------------------------\n
 *  or \n
 * -------------------------------------------------------------------------------------------------------------\n
 * | "account_logout"(char*) | msglen(uint32_t) | fail(char) | error message | ... |                            \n
 * -------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see jxmemcpy for return code.
 *
 * @sa jxmemcpy
 */
int am_account_logout_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    int ret = 1;
    uint32_t len_result = 0;

    if (inmsg[0] == g_succeed) {
        fprintf(stdout, "client: logout succeed.\n");
        if (inmsg[1] != 0) {
            fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
            if (outmsg != NULL) {
                len_result = (int)strlen(inmsg) + 1;
                jxsprintf(outmsg, *outmsglen, "account_logout%c%c%c%c%c%s", TO4C(len_result), g_succeed, inmsg + 1);
            }
        } else if (outmsg != NULL) {
            len_result = 1;
            jxsprintf(outmsg, *outmsglen, "account_logout%c%c%c%c%c", TO4C(len_result), g_succeed);
        }
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: account_logout fail, %s\n", inmsg + 1);
        if (outmsg != NULL) {
            len_result = 1 + (int)strlen(inmsg + 1) + 1;
            jxsprintf(outmsg, *outmsglen, "account_logout%c%c%c%c%c%s", TO4C(len_result), g_fail, inmsg + 1);
        }
        return 1;
    } else {
        fprintf(stderr, "client: account_logout - unkown message.\n");
        return -2;
    }

    return ret;
}

/**
 * @brief  am_account_changeusername_react The message from server is to tell client whether or not update new username.
 *
 * @param inmsg The format of inmsg is \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------\n
 * | succeed(char) | additional message | ... |                            \n
 * ------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg the format of outmsg is: \n
 * --------------------------------------------------------------------------------------------------------\n
 * | "account_changeusername"(char*) | msglen(uint32_t) | succeed(char) | ... |                            \n
 * --------------------------------------------------------------------------------------------------------\n
 *  or \n
 * ---------------------------------------------------------------------------------------------------------------------\n
 * | "account_changeusername"(char*) | msglen(uint32_t) | fail(char) | error message | ... |                            \n
 * ---------------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen Not used.
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see jxmemcpy for return code.
 *
 * @sa jxmemcpy
 */
int am_account_changeusername_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    int ret = 1;
    uint32_t len_result = 0;

    if (inmsg[0] == g_succeed) {
        if ((ret = jxmemcpy(
                        &s_account_client.account_basic.username,
                        sizeof(s_account_client.account_basic.username),
                        &s_account_tmp.account_basic.username,
                        sizeof(s_account_tmp.account_basic.username))) != 0) {
            fprintf(stderr, "client: account_changeusername fail. please check the buffer size of local account username.\n");
            return ret;
        }

        fprintf(stdout, "client: account_changeusername succeed.\n");

        if (inmsg[1] != 0) {
            fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
            if (outmsg != NULL) {
                len_result = (int)strlen(inmsg) + 1;
                jxsprintf(
                        outmsg,
                        *outmsglen,
                        "account_changeusername%c%c%c%c%c%s",
                        TO4C(len_result),
                        g_succeed, inmsg + 1);
            }
        } else if (outmsg != NULL) {
            len_result = 1;
            jxsprintf(outmsg, *outmsglen, "account_changeusername%c%c%c%c%c", TO4C(len_result), g_succeed);
        }
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: account_changeusername fail, %s\n", inmsg + 1);
        if (outmsg != NULL) {
            len_result = 1 + (int)strlen(inmsg + 1) + 1;
            jxsprintf(outmsg, *outmsglen, "account_changeusername%c%c%c%c%c%s", TO4C(len_result), g_fail, inmsg + 1);
        }
        return 1;
    } else {
        fprintf(stderr, "client: account_changeusername - unkown message.\n");
        return -2;
    }

    return ret;
}

/**
 * @brief  am_account_changepasswd_react The message from server is to tell client whether or not update new passwd.
 *
 * @param inmsg The format of inmsg is \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------\n
 * | succeed(char) | additional message | ... |                            \n
 * ------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg the format of outmsg is: \n
 * ------------------------------------------------------------------------------------------------------\n
 * | "account_changepasswd"(char*) | msglen(uint32_t) | succeed(char) | ... |                            \n
 * ------------------------------------------------------------------------------------------------------\n
 *  or \n
 * -------------------------------------------------------------------------------------------------------------------\n
 * | "account_changepasswd"(char*) | msglen(uint32_t) | fail(char) | error message | ... |                            \n
 * -------------------------------------------------------------------------------------------------------------------\n
 * @param outmsglen Not used.
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see jxmemcpy for return code.
 *
 * @sa jxmemcpy
 */
int am_account_changepasswd_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    int ret = 1;
    uint32_t len_result = 0;

    if (inmsg[0] == g_succeed) {
        if ((ret = jxmemcpy(
                        &s_account_client.passwd,
                        sizeof(s_account_client.passwd),
                        &s_account_tmp.passwd,
                        sizeof(s_account_tmp.passwd))) != 0) {
            fprintf(stderr, "client: account_changepasswd fail. please check the buffer size of local account passwd.\n");
            return ret;
        }

        fprintf(stdout, "client: account_changepasswd succeed.\n");

        if (inmsg[1] != 0) {
            fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
            if (outmsg != NULL) {
                len_result = (int)strlen(inmsg) + 1;
                jxsprintf(outmsg, *outmsglen, "account_changepasswd%c%c%c%c%c%s", TO4C(len_result), g_succeed, inmsg + 1);
            }
        } else if (outmsg != NULL) {
            len_result = 1;
            jxsprintf(outmsg, *outmsglen, "account_changepasswd%c%c%c%c%c", TO4C(len_result), g_succeed);
        }
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: account_changepasswd fail, %s\n", inmsg + 1);
        if (outmsg != NULL) {
            len_result = 1 + (int)strlen(inmsg + 1) + 1;
            jxsprintf(outmsg, *outmsglen, "account_changepasswd%c%c%c%c%c%s", TO4C(len_result), g_fail, inmsg + 1);
        }
        return 1;
    } else {
        fprintf(stderr, "client: account_changepasswd - unkown message.\n");
        return -2;
    }

    return ret;
}

/**
 * @brief  am_account_changegrade_react The message from server is to tell client whether or not update new grade.
 *
 * @param inmsg The format of inmsg is \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------\n
 * | succeed(char) | additional message | ... |                            \n
 * ------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg the format of outmsg is: \n
 * -----------------------------------------------------------------------------------------------------\n
 * | "account_changegrade"(char*) | msglen(uint32_t) | succeed(char) | ... |                            \n
 * -----------------------------------------------------------------------------------------------------\n
 *  or \n
 * ------------------------------------------------------------------------------------------------------------------\n
 * | "account_changegrade"(char*) | msglen(uint32_t) | fail(char) | error message | ... |                            \n
 * ------------------------------------------------------------------------------------------------------------------\n
 *
 * @param outmsglen Not used.
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see jxmemcpy for return code.
 *
 * @sa jxmemcpy
 */
int am_account_changegrade_react(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    uint32_t len_result = 0;

    if (inmsg[0] == g_succeed) {
        s_account_client.account_basic.grade = s_account_tmp.account_basic.grade;
        fprintf(stdout, "client: account_changegrade succeed.\n");

        if (inmsg[1] != 0) {
            if (outmsg != NULL) {
                len_result = (int)strlen(inmsg) + 1;
                jxsprintf(outmsg, *outmsglen, "account_changegrade%c%c%c%c%c%s", TO4C(len_result), g_succeed, inmsg + 1);
            }
            fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
        } else if (outmsg != NULL) {
            len_result = 1;
            jxsprintf(outmsg, *outmsglen, "account_changegrade%c%c%c%c%c", TO4C(len_result), g_succeed);
        }
    } else if (inmsg[0] == g_fail) {
        fprintf(stderr, "client: account_changegrade fail, %s\n", inmsg + 1);
        if (outmsg != NULL) {
            len_result = 1 + (int)strlen(inmsg + 1) + 1;
            jxsprintf(outmsg, *outmsglen, "account_changegrade%c%c%c%c%c%s", TO4C(len_result), g_fail, inmsg + 1);
        }
        return 1;
    } else {
        fprintf(stderr, "client: account_changegrade - unkown message.\n");
        return -2;
    }

    return 0;
}

#undef TO4C

