/**
 * @file client_msgdispatch.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-07
 * @modified  Sat 2015-11-07 14:46:59 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#endif

#include  <stdio.h>
#include  <string.h>
#include  <stdint.h>
#include  <semaphore.h>
#include    "lightthread.h"
#include    "sock_types.h"
#include    "utility_wrap.h"
#include    "bufarray.h"
#include    "msgpool.h"
#include    "msgwrap.h"
#include    "client.h"


#ifdef __cplusplus
extern "C"
{
#endif

extern const char* g_loginmsg_header;
extern const char* g_logoutmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static void s_process_login_sessin(char* inmsg, char* outmsg, int* outmsglen, struct csclient* cli);
static void s_process_communication(char* inmsg, char* outmsg, int* outmsglen, struct csclient* cli);

#ifdef __cplusplus
}
#endif


void csclient_process_msg(char* inmsg, char* outmsg, int* outmsglen, void* clidata)
{
	struct csclient* cli = (struct csclient*)clidata;

	if (!cli->loggedin) {
		s_process_login_sessin(inmsg, outmsg, outmsglen, cli);
	} else {
		s_process_communication(inmsg, outmsg, outmsglen, cli);
	}
}

void s_process_login_sessin(char* inmsg, char* outmsg, int* outmsglen, struct csclient* cli)
{
    char* msgdata = NULL;

    (void)outmsg;
    (void)outmsglen;

	msgdata = inmsg + sizeof(struct csmsg_header);
	if (strncmp(msgdata, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1) == 0) {
        cli->loggedin = 1;
		printf("%s switch to login interface.\n", cli->prompt);
	} else {
		printf("%s error username or password.\n", cli->prompt);
	}
}

void s_process_communication(char* inmsg, char* outmsg, int* outmsglen, struct csclient* cli)
{
    char* msgdata = NULL;

	msgdata = inmsg + sizeof(struct csmsg_header);

	if (strncmp(msgdata, g_logoutmsg_header, strlen(g_logoutmsg_header) + 1) == 0) {
        cli->loggedin = 0;
		printf("%s switch to logout interface.\n", cli->prompt);
	} else {
		printf("%s prcess \"%s\" from server.\n", cli->prompt, msgdata);
		cs_memcpy(outmsg, *outmsglen, msgdata, ((struct csmsg_header*)inmsg)->numbytes);
		*outmsglen = ((struct csmsg_header*)inmsg)->numbytes;
	}
}

