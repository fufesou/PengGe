/**
 * @file client_account.c
 * @brief  This file defines some basic account process functions for client. 
 * Some message formats are described in the document:
 * -----------------------------------------------
 * | data name(type) | (*) data name(type) | ... |
 * -----------------------------------------------
 * 1. The data name is the name of this segment, such as 'username'.
 * 2. The type describe the segement's type, such as 'int8'.
 * 3. (*) means this segement is not filled with data by now.
 * 4. ... means unused remainder place.
 *  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  Mon 2015-11-16 01:56:15 (+0800)
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

static uint32_t s_idlen = sizeof(uint32_t) + sizeof(uint32_t);

static struct account_client_t
{
	struct account_basic_t account_basic;
	char passwd[ACCOUNT_PASSWD_LEN];
} s_account_client, s_account_tmp;

inline static int s_fill_ids(const char* name_request, char* outmsg, uint32_t outmsglen);


#ifdef __cplusplus
}
#endif

int s_fill_ids(const char* name_request, char* outmsg, uint32_t outmsglen)
{
	if (outmsglen < s_idlen)
	{
		return 1;
	}
	*(uint32_t*)(outmsg) = htonl(s_account_client.account_basic.id);
	*(uint32_t*)(outmsg + sizeof(uint32_t)) = htonl(am_method_getid(name_request));

	return 0;
}


/**************************************************
 **             the request block                 **
 **************************************************/
int am_account_create_request(const char* tel, char* outmsg, uint32_t* outmsglen)
{
	uint32_t len_tel = 0;

	if (s_fill_ids("account_create", outmsg, *outmsglen)) {
		return 1;
	}

	len_tel = strlen(tel);
	if (cs_memcpy(outmsg + s_idlen, *outmsglen - s_idlen, tel, len_tel + 1) != 0) {
		return 1;
	}

	*outmsglen = s_idlen + len_tel + 1;
	return 0;
}

int am_account_login_request(const char* username_tel, const char* passwd, char* outmsg, uint32_t* outmsglen)
{	
	uint32_t len_username_tel = 0;
	uint32_t len_passwd = 0;

	if (s_fill_ids("account_login", outmsg, *outmsglen)) {
		return 1;
	}

	len_username_tel = strlen(username_tel);
	len_passwd = strlen(passwd);

	if (cs_memcpy(outmsg + s_idlen, *outmsglen - s_idlen, username_tel, len_username_tel + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(outmsg + s_idlen + len_username_tel + 1, *outmsglen - s_idlen - len_username_tel - 1, passwd, len_passwd + 1) != 0) {
		return 1;
	}
	*outmsglen = s_idlen + len_username_tel + len_passwd + 2;
    cs_memcpy(s_account_client.passwd, sizeof(s_account_client.passwd), passwd, len_passwd + 1);

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

	if (s_fill_ids("account_changeusername", outmsg, *outmsglen)) {
		return 1;
	}

	len_username_old = strlen(username_old);
	len_username_new = strlen(username_new);
	len_passwd = strlen(passwd);

	if (cs_memcpy(outmsg + s_idlen, *outmsglen - s_idlen, username_old, len_username_old + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(outmsg + s_idlen + len_username_old + 1, *outmsglen - s_idlen - len_username_old - 1, passwd, len_passwd + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(outmsg + s_idlen + len_username_old + len_passwd + 2, *outmsglen - s_idlen - len_username_old - len_passwd - 2, username_new, len_username_new + 1) != 0) {
		return 1;
	}
	*outmsglen = s_idlen + len_username_old + len_passwd + len_username_new + 3;

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

	if (s_fill_ids("account_changepasswd", outmsg, *outmsglen)) {
		return 1;
	}

	len_username = strlen(username);
	len_passwd_old = strlen(passwd_old);
	len_passwd_new = strlen(passwd_new);

	if (cs_memcpy(outmsg + s_idlen, *outmsglen - s_idlen, username, len_username + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(outmsg + s_idlen + len_username + 1, *outmsglen - s_idlen - len_username - 1, passwd_old, len_passwd_old + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(outmsg + s_idlen + len_username + len_passwd_old + 2, *outmsglen - s_idlen - len_username - len_passwd_old - 2, passwd_old, len_passwd_new + 1) != 0) {
		return 1;
	}
	*outmsglen = s_idlen + len_username + len_passwd_old + len_passwd_new + 3;

	cs_memcpy(s_account_tmp.passwd, sizeof(s_account_tmp.passwd), passwd_new, strlen(passwd_new) + 1);

	return 0;
}

int am_account_changegrade_request(
			const char* username,
			const char* passwd,
			uint32_t grade,
			char* outmsg,
            uint32_t* outmsglen)
{
	uint32_t len_username = 0;
	uint32_t len_passwd = 0;

	if (s_fill_ids("account_grade", outmsg, *outmsglen)) {
		return 1;
	}

	len_username = strlen(username);
	len_passwd = strlen(passwd);

	if (cs_memcpy(outmsg + s_idlen, *outmsglen - s_idlen, username, len_username + 1) != 0) {
		return 1;
	}
	if (cs_memcpy(outmsg + s_idlen + len_username + 1, *outmsglen - s_idlen - len_username - 1, passwd, len_passwd + 1) != 0) {
		return 1;
	}
	*(uint32_t*)(outmsg + s_idlen + len_username + len_passwd + 2) = htonl(grade);
	*outmsglen = s_idlen + len_username + len_passwd + 2 + sizeof(uint32_t);

	s_account_tmp.account_basic.grade = grade;

	return 0;
}


/**************************************************
 **             the reply block                 **
 **************************************************/
/**
 * @brief  am_account_create_react This is the first message returned from server. And this message is just a notification message.
 *
 * @param inmsg
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
 * @brief  am_account_login_react This function estimate whether the login succeed. 
 *
 * @param inmsg 
 *
 * @param inmsglen
 * @param outmsg 
 * @param outmsglen
 *
 * @return   0 if an account is created, 1 otherwise.
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
    } else {
		fprintf(stderr, "client: login fail.\n");
	}

    return ret;
}

/**
 * @brief  am_account_changeusername_react The message from server is to tell client whether or not update new username.
 *
 * @param inmsg The format of inmsg is
 * -----------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | succeed(char) | ... | 
 * -----------------------------------------------------------------
 *  or
 * --------------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | succeed(char) | additional message | ... |
 * --------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * ------------------------------------------------------------------------------
 *
 * @param outmsg Not used.
 * @param outmsglen Not used.
 *
 * @return   
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
    }
	else {
		fprintf(stderr, "client: change username fail. message: %s.\n", inmsg);
		return 1;
	}
    return ret;
}

/**
 * @brief  am_account_changepasswd_react The message from server is to tell client whether or not update new passwd.
 *
 * @param inmsg The format of inmsg is
 * -----------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | succeed(char) | ... | 
 * -----------------------------------------------------------------
 *  or
 * --------------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | succeed(char) | additional message | ... |
 * --------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * ------------------------------------------------------------------------------
 *
 * @param outmsg Not used.
 * @param outmsglen Not used.
 *
 * @return   
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
    }
	else {
		fprintf(stderr, "client: change passwd fail. message: %s.\n", inmsg);
		return 1;
	}
    return ret;
}

/**
 * @brief  am_account_changegrade_react The message from server is to tell client whether or not update new grade.
 *
 * @param inmsg The format of inmsg is
 * -----------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | succeed(char) | ... | 
 * -----------------------------------------------------------------
 *  or
 * --------------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | succeed(char) | additional message | ... |
 * --------------------------------------------------------------------------------------
 *  or
 * ------------------------------------------------------------------------------
 * | user id(uint32_t) | process id(int32_t) | fail(char) | error message | ... |
 * ------------------------------------------------------------------------------
 *
 * @param outmsg Not used.
 * @param outmsglen Not used.
 *
 * @return   
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
    }
	else {
		fprintf(stderr, "client: change passwd fail. message: %s.\n", inmsg);
		return 1;
	}
    return 0;
}

