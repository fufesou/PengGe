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
 * @modified  Sun 2015-11-15 16:10:02 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#endif

#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include    "config_macros.h"
#include    "account_macros.h"
#include    "utility_wrap.h"
#include    "sock_types.h"
#include    "msgwrap.h"
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


static uint32_t s_headerlen = sizeof(struct csmsg_header) + sizeof(uint32_t) + sizeof(int32_t);


int s_genera_msg_create(const char* inmsg, uint32_t userid, uint32_t methodid, char* outmsg, __inout uint32_t outmsglen);


#ifdef __cplusplus
}
#endif


/**
 * @brief  s_general_msg_create 
 *
 * @param inmsg The format of inmsg should be
 * --------------------------------------------------------------------------------------------------------
 * | struct csmsg_header | (*) user id(uint32_t) | (*) process id(int32_t) | user input data(char*) | ... |
 * --------------------------------------------------------------------------------------------------------
 *
 * @param userid The account userid to fill 'user id' message segment.
 * @param methodid The methodid is respond to the id ot request method which is fetched from by 'am_method_getid("method name")'.
 * @param outmsg The out message is just the same as inmsg expect for the filled 'user id' and 'process id'.
 * @param outmsglen
 *
 * @return   
 */
int s_general_msg_create(const char* inmsg, uint32_t userid, uint32_t methodid, char* outmsg, __inout uint32_t outmsglen)
{
	if (cs_memcpy(outmsg, outmsglen, inmsg, GET_HEADER_DATA(inmsg, numbytes)) != 0) {
		return 1;
	}

    *(uint32_t*)(outmsg + sizeof(struct csmsg_header)) = htonl(userid);
    *(uint32_t*)(outmsg + sizeof(struct csmsg_header) + sizeof(userid)) = htonl(methodid);
    return 0;
}

/**************************************************
 **             the request block                 **
 **************************************************/
int am_account_create_request(const char* inmsg, uint32_t userid, char* outmsg, uint32_t outmsglen)
{
	return s_general_msg_create(inmsg, userid, am_method_getid("account_create"), outmsg, outmsglen); 
}

int am_account_login_request(const char* inmsg, uint32_t userid, char* outmsg, uint32_t outmsglen)
{	
	char* passwd = strchr(inmsg + s_headerlen, '\0') + 1;
	snprintf(s_account_client.passwd, sizeof(s_account_client.passwd), passwd, strlen(passwd) + 1);

	return s_general_msg_create(inmsg, userid, am_method_getid("account_login"), outmsg, outmsglen); 
}

int am_account_changeusername_request(const char* inmsg, uint32_t userid, char* outmsg, uint32_t outmsglen)
{
	const char* passwd = strchr(inmsg + s_headerlen, '\0');
	const char* username_new = strchr(passwd + 1, '\0');

	cs_memcpy(s_account_tmp.account_basic.username, sizeof(s_account_tmp.account_basic.username), username_new, strlen(username_new) + 1);

	return s_general_msg_create(inmsg, userid, am_method_getid("account_changeusername"), outmsg, outmsglen); 
}

int am_account_changepasswd_request(const char* inmsg, uint32_t userid, char* outmsg, uint32_t outmsglen)
{
	const char* passwd_old = strchr(inmsg + s_headerlen, '\0');
	const char* passwd_new = strchr(passwd_old + 1, '\0');

    cs_memcpy(s_account_tmp.passwd, sizeof(s_account_tmp.passwd), passwd_new, strlen(passwd_new) + 1);

	return s_general_msg_create(inmsg, userid, am_method_getid("account_changepasswd"), outmsg, outmsglen); 
}

int am_account_changegrade_request(const char* inmsg, uint32_t userid, char* outmsg, uint32_t outmsglen)
{
	const char* passwd = strchr(inmsg + s_headerlen, '\0');
    s_account_tmp.account_basic.grade = ntohl(*(uint32_t*)(strchr(passwd, '\0') + 1));

	return s_general_msg_create(inmsg, userid, am_method_getid("account_changepasswd"), outmsg, outmsglen); 
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

    if (inmsg[s_headerlen] == g_succeed) {
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

    if (inmsg[s_headerlen] == g_succeed) {
        if ((ret = (cs_memcpy(&s_account_client.account_basic, sizeof(struct account_basic_t), inmsg + s_headerlen + 1, sizeof(struct account_basic_t)))) != 0) {
			fprintf(stderr, "client: login suceed, but client cannot update account data.\n");
			return ret;
		}

		fprintf(stdout, "client: login succeed.\n");

		if (inmsg[s_headerlen + 1 + sizeof(struct account_basic_t)] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + s_headerlen + 1 + sizeof(struct account_basic_t));
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

    if (inmsg[s_headerlen] == g_succeed) {
        if ((ret = cs_memcpy(
						&s_account_client.account_basic.username,
						sizeof(s_account_client.account_basic.username),
						&s_account_tmp.account_basic.username,
                        sizeof(s_account_tmp.account_basic.username))) != 0) {
			fprintf(stderr, "client: change username fail. please check the buffer size of local account username.\n");
			return ret;
		}

		fprintf(stdout, "client: change username succeed.\n");

		if (inmsg[s_headerlen + 1] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + s_headerlen + 1);
		}
    }
	else {
		fprintf(stderr, "client: change username fail. message: %s.\n", inmsg + s_headerlen);
		return 1;
	}
    return ret;
}

/**
 * @brief  am_account_changepasswd_react The message from server is to tell client whether or not update new passwd.
 *
 * @param inmsg The format of inmsg is
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

    if (inmsg[s_headerlen] == g_succeed) {
        if ((ret = cs_memcpy(
                        &s_account_client.passwd,
                        sizeof(s_account_client.passwd),
                        &s_account_tmp.passwd,
                        sizeof(s_account_tmp.passwd))) != 0) {
			fprintf(stderr, "client: change passwd fail. please check the buffer size of local account passwd.\n");
			return ret;
		}

		fprintf(stdout, "client: change passwd succeed.\n");

		if (inmsg[s_headerlen + 1] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + s_headerlen + 1);
		}
    }
	else {
		fprintf(stderr, "client: change passwd fail. message: %s.\n", inmsg + s_headerlen);
		return 1;
	}
    return ret;
}

/**
 * @brief  am_account_changegrade_react The message from server is to tell client whether or not update new grade.
 *
 * @param inmsg The format of inmsg is
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
 * @param outmsg Not used.
 * @param outmsglen Not used.
 *
 * @return   
 */
int am_account_changegrade_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen)
{
    (void)outmsg;
	(void)outmsglen;

    if (inmsg[s_headerlen] == g_succeed) {
		s_account_client.account_basic.grade = s_account_tmp.account_basic.grade;
		fprintf(stdout, "client: change passwd succeed.\n");

		if (inmsg[s_headerlen + 1] != 0) {
			fprintf(stdout, "client: additional message from server - %s.\n", inmsg + s_headerlen + 1);
		}
    }
	else {
		fprintf(stderr, "client: change passwd fail. message: %s.\n", inmsg + s_headerlen);
		return 1;
	}
    return 0;
}

