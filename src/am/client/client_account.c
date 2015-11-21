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
 * @modified  Fri 2015-11-20 23:45:49 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <netinet/in.h>
#endif

#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include    "config_macros.h"
#include    "account_macros.h"
#include    "utility_wrap.h"
#include    "account.h"


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
int am_account_create_request(const char* tel, char* outmsg, uint32_t* outmsglen)
{
	uint32_t len_tel = 0;
	uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
		return 1;
	}
    *(uint32_t*)outmsg = htonl(am_method_getid("account_create"));

	len_tel = strlen(tel);
    msglen = sizeof(uint32_t) + len_tel + 1;

    if (*outmsglen < msglen) {
		return 1;
	}
    if (cs_memcpy(outmsg + sizeof(uint32_t), *outmsglen - sizeof(uint32_t), tel, len_tel + 1) != 0) {
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
    if (cs_memcpy(outmsg + sizeof(uint32_t), *outmsglen - sizeof(uint32_t), tel, len_tel + 1) != 0) {
		return 1;
	}
    if (cs_memcpy(outmsg + sizeof(uint32_t) + len_tel + 1, *outmsglen - sizeof(uint32_t) - len_randcode - 1, randcode, len_randcode + 1) != 0) {
		return 1;
	}

	*outmsglen = msglen;
	return 0;
}

int am_account_login_request(const char* username_tel, const char* passwd, char* outmsg, uint32_t* outmsglen)
{	
	uint32_t len_username_tel = 0;
	uint32_t len_passwd = 0;
	uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
		return 1;
	}
    *(uint32_t*)outmsg = htonl(am_method_getid("account_login"));

	len_username_tel = strlen(username_tel);
	len_passwd = strlen(passwd);
    msglen = sizeof(uint32_t) + len_username_tel + len_passwd + 2;

	if (*outmsglen < msglen) {
		return 1;
	}

    if (cs_memcpy(outmsg + sizeof(uint32_t), *outmsglen - sizeof(uint32_t), username_tel, len_username_tel + 1) != 0) {
		return 1;
	}
    if (cs_memcpy(outmsg + sizeof(uint32_t) + len_username_tel + 1, *outmsglen - sizeof(uint32_t) - len_username_tel - 1, passwd, len_passwd + 1) != 0) {
		return 1;
	}
	*outmsglen = msglen;
    cs_memcpy(s_account_client.passwd, sizeof(s_account_client.passwd), passwd, len_passwd + 1);

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
			const char* username_old,
			const char* passwd,
			const char* username_new,
			char* outmsg,
            uint32_t* outmsglen)
{
	uint32_t len_username_old = 0;
	uint32_t len_username_new = 0;
	uint32_t len_passwd = 0;
	uint32_t msglen = 0;

    if (*outmsglen < (sizeof(uint32_t) * 2)) {
		return 1;
	}
    *(uint32_t*)outmsg = htonl(am_method_getid("account_changeusername"));
    *(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(s_account_client.account_basic.id);

	len_username_old = strlen(username_old);
	len_username_new = strlen(username_new);
	len_passwd = strlen(passwd);
    msglen = sizeof(uint32_t) * 2 + len_username_old + len_passwd + len_username_new + 3;

	if (*outmsglen < msglen) {
		return 1;
	}

    if (cs_memcpy(outmsg + sizeof(uint32_t) * 2, *outmsglen - sizeof(uint32_t) * 2, username_old, len_username_old + 1) != 0) {
		return 1;
	}
    if (cs_memcpy(outmsg + sizeof(uint32_t) * 2 + len_username_old + 1, *outmsglen - sizeof(uint32_t) * 2 - len_username_old - 1, passwd, len_passwd + 1) != 0) {
		return 1;
	}
    if (cs_memcpy(outmsg + sizeof(uint32_t) * 2 + len_username_old + len_passwd + 2, *outmsglen - sizeof(uint32_t) * 2 - len_username_old - len_passwd - 2, username_new, len_username_new + 1) != 0) {
		return 1;
	}
	*outmsglen = msglen;

	cs_memcpy(s_account_tmp.account_basic.username, sizeof(s_account_tmp.account_basic.username), username_new, strlen(username_new) + 1);

	return 0;
}

int am_account_changepasswd_request(
			const char* username,
			const char* passwd_old,
			const char* passwd_new,
			char* outmsg,
            uint32_t* outmsglen)
{
	uint32_t len_username = 0;
	uint32_t len_passwd_old = 0;
	uint32_t len_passwd_new = 0;
	uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
		return 1;
	}
    *(uint32_t*)outmsg = htonl(am_method_getid("account_changepasswd"));
    *(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(s_account_client.account_basic.id);

	len_username = strlen(username);
	len_passwd_old = strlen(passwd_old);
	len_passwd_new = strlen(passwd_new);
    msglen = sizeof(uint32_t) * 2 + len_username + len_passwd_old + len_passwd_new + 3;

	if (*outmsglen < msglen) {
		return 1;
	}

	if (cs_memcpy(
                    outmsg + sizeof(uint32_t) * 2,
                    *outmsglen - sizeof(uint32_t) * 2,
					username,
					len_username + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(
                    outmsg + sizeof(uint32_t) * 2 + len_username + 1,
                    *outmsglen - sizeof(uint32_t) * 2 - len_username - 1,
					passwd_old,
					len_passwd_old + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(
                    outmsg + sizeof(uint32_t) * 2 + len_username + len_passwd_old + 2,
                    *outmsglen - sizeof(uint32_t) * 2 - len_username - len_passwd_old - 2,
					passwd_old,
					len_passwd_new + 1) != 0) {
		return 1;
	}
	*outmsglen = msglen;

	cs_memcpy(s_account_tmp.passwd, sizeof(s_account_tmp.passwd), passwd_new, strlen(passwd_new) + 1);

	return 0;
}

int am_account_changegrade_request(
			const char* username,
			const char* passwd,
			uint8_t grade,
			char* outmsg,
            uint32_t* outmsglen)
{
	uint32_t len_username = 0;
	uint32_t len_passwd = 0;
	uint32_t msglen = 0;

    if (*outmsglen < sizeof(uint32_t)) {
		return 1;
	}
    *(uint32_t*)outmsg = htonl(am_method_getid("account_changepasswd"));
    *(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(s_account_client.account_basic.id);

	len_username = strlen(username);
	len_passwd = strlen(passwd);
    msglen = sizeof(uint32_t) * 2 + len_username + len_passwd + 2 + sizeof(grade);
	if (*outmsglen < msglen) {
		return 1;
	}

    if (cs_memcpy(outmsg + sizeof(uint32_t) * 2, *outmsglen - sizeof(uint32_t) * 2, username, len_username + 1) != 0) {
		return 1;
	}
    if (cs_memcpy(outmsg + sizeof(uint32_t) * 2 + len_username + 1, *outmsglen - sizeof(uint32_t) * 2 - len_username - 1, passwd, len_passwd + 1) != 0) {
		return 1;
	}
    *(uint8_t*)(outmsg + sizeof(uint32_t) * 2 + len_username + len_passwd + 2) = grade;
	*outmsglen = msglen;

	s_account_tmp.account_basic.grade = grade;

	return 0;
}


/**************************************************
 **             the reply block                 **
 **************************************************/
/**
 * @brief  am_account_create_react This is the first message returned from server. And this message is just a notification message.
 *
 * @param inmsg The format of inmsg is: \n
 * ---------------------------------------------------\n
 * | succeed(char) | ... |                            \n
 * ---------------------------------------------------\n
 *  or \n
 * ------------------------------------------------\n
 * | fail(char) | ... |                            \n
 * ------------------------------------------------\n
 *
 * @param outmsg unused.
 * @param outmsglen unused.
 *
 * @return   The return value has no meaning, and it always is 0.
 */
int am_account_create_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    (void)outmsg;
    (void)outmsglen;

    if (inmsg[0] == g_succeed) {
        printf("client: create account succeed, please wait for a moment to receiving the random telcode, then revify it.\n");
    } else {
        printf("client: create account fail.\n");
    }

    return 0;
}

/**
 * @brief  am_account_verify_react 
 *
 * @param inmsg The format of outmsg here is: \n
 * -------------------------------------------------------------------------------------\n
 * | succeed(char) | account(struct account_basic_t) | ... |                            \n
 * -------------------------------------------------------------------------------------\n
 *  or \n
 * ----------------------------------------------------------------\n
 * | fail(char) | error message | ... |                            \n
 * ----------------------------------------------------------------\n
 *
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see cs_memcpy for return code.
 *
 * @sa cs_memcpy
 */
int am_account_verify_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    int ret = 1;

    (void)outmsg;
    (void)outmsglen;

    if (inmsg[0] == g_succeed) {
        if ((ret = (cs_memcpy(&s_account_client.account_basic, sizeof(struct account_basic_t), inmsg + 1, sizeof(struct account_basic_t)))) != 0) {
			fprintf(stderr, "client: verify suceed, but client cannot update account data.\n");
			return ret;
		}

        fprintf(stdout, "client: verify succeed");

		if (inmsg[1 + sizeof(struct account_basic_t)] != 0) {
            fprintf(stdout, ", additional message from server - %s.\n", inmsg + 1 + sizeof(struct account_basic_t));
        } else {
            fprintf(stdout, ".\n");
        }
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: verify fail, %s\n", inmsg + 1);
        return 1;
    } else {
        fprintf(stderr, "client: account_verify - unkown message.\n");
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
 * @param inmsglen
 * @param outmsg 
 * @param outmsglen
 *
 * @return
 * - 0 if succeed.
 * - 1 if fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see cs_memcpy for return code.
 *
 * @sa cs_memcpy
 */
int am_account_login_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    int ret = 1;

    (void)outmsg;
    (void)outmsglen;

    if (inmsg[0] == g_succeed) {
        if ((ret = (cs_memcpy(&s_account_client.account_basic, sizeof(struct account_basic_t), inmsg + 1, sizeof(struct account_basic_t)))) != 0) {
			fprintf(stderr, "client: login suceed, but client cannot update account data.\n");
			return ret;
		}

		fprintf(stdout, "client: login succeed.\n");

		if (inmsg[1 + sizeof(struct account_basic_t)] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1 + sizeof(struct account_basic_t));
		}
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: login fail, %s\n", inmsg + 1);
        return 1;
    } else {
        fprintf(stderr, "client: account_login - unkown message.\n");
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
 * @param outmsg
 * @param outmsglen
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see cs_memcpy for return code.
 *
 * @sa cs_memcpy
 */
int am_account_logout_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    int ret = 1;

    (void)outmsg;
	(void)outmsglen;

    if (inmsg[0] == g_succeed) {
		fprintf(stdout, "client: logout succeed.\n");
		if (inmsg[1] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
		}
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: logout fail, %s\n", inmsg + 1);
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
 * @param outmsg Not used.
 * @param outmsglen Not used.
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see cs_memcpy for return code.
 *
 * @sa cs_memcpy
 */
int am_account_changeusername_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    int ret = 1;

    (void)outmsg;
	(void)outmsglen;

    if (inmsg[0] == g_succeed) {
        if ((ret = cs_memcpy(
						&s_account_client.account_basic.username,
						sizeof(s_account_client.account_basic.username),
						&s_account_tmp.account_basic.username,
                        sizeof(s_account_tmp.account_basic.username))) != 0) {
			fprintf(stderr, "client: change username fail. please check the buffer size of local account username.\n");
			return ret;
		}

		fprintf(stdout, "client: change username succeed.\n");

		if (inmsg[1] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
		}
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: chagneusername fail, %s\n", inmsg + 1);
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
 * @param outmsg Not used.
 * @param outmsglen Not used.
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see cs_memcpy for return code.
 *
 * @sa cs_memcpy
 */
int am_account_changepasswd_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    int ret = 1;

    (void)outmsg;
	(void)outmsglen;

    if (inmsg[0] == g_succeed) {
        if ((ret = cs_memcpy(
                        &s_account_client.passwd,
                        sizeof(s_account_client.passwd),
                        &s_account_tmp.passwd,
                        sizeof(s_account_tmp.passwd))) != 0) {
			fprintf(stderr, "client: change passwd fail. please check the buffer size of local account passwd.\n");
			return ret;
		}

		fprintf(stdout, "client: change passwd succeed.\n");

		if (inmsg[1] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
		}
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: changepasswd fail, %s\n", inmsg + 1);
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
 * @param outmsg Not used.
 * @param outmsglen Not used.
 *
 * @return   
 * - 0 if succeed.
 * - 1 if verify fail.
 * - -2 if message is unkown.
 * - others, if copy account fail, see cs_memcpy for return code.
 *
 * @sa cs_memcpy
 */
int am_account_changegrade_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    (void)outmsg;
	(void)outmsglen;

    if (inmsg[0] == g_succeed) {
		s_account_client.account_basic.grade = s_account_tmp.account_basic.grade;
		fprintf(stdout, "client: change passwd succeed.\n");

		if (inmsg[1] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + 1);
		}
    } else if (inmsg[0] == g_fail){
        fprintf(stderr, "client: changegrade fail, %s\n", inmsg + 1);
        return 1;
    } else {
        fprintf(stderr, "client: account_changegrade - unkown message.\n");
        return -2;
    }

    return 0;
}

