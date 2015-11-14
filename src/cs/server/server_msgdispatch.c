/**
 * @file server_msgdispatch.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 * @modified  Sat 2015-11-07 13:22:11 (+0800)
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


#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_loginmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static void s_process_login(char* inmsg, char* outmsg, uint32_t* outmsglen);
static int s_login_verification(const char *username, const char *password);
static void s_process_communication(char* inmsg, char* outmsg, uint32_t* outmsglen);

#ifdef __cplucplus
}
#endif


void csserver_process_msg(char* inmsg, char* outmsg, uint32_t* outmsglen, void* unused)
{
	(void)unused;

    if (strncmp(inmsg + sizeof(struct csmsg_header), g_loginmsg_header, strlen(g_loginmsg_header)) == 0) {
        s_process_login(inmsg, outmsg, outmsglen);
    }
    else {
        s_process_communication(inmsg, outmsg, outmsglen);
    }
}

void s_process_login(char* inmsg, char* outmsg, uint32_t* outmsglen)
{
    char* delimiter = NULL;
    const char* username = NULL;
    const char* password = NULL;
    const char* loginres = NULL;
    uint32_t loginmsg_header_len = 0;
    uint32_t size_msghdr = sizeof(struct csmsg_header);

    loginmsg_header_len = strlen(g_loginmsg_header);
    delimiter = strchr(inmsg + size_msghdr + loginmsg_header_len, g_login_delimiter);
    username = inmsg + size_msghdr + loginmsg_header_len;
    *delimiter = '\0';
    password = delimiter + 1;

    loginres = s_login_verification(username, password) ? g_loginmsg_SUCCESS : g_loginmsg_FAIL;
    cs_memcpy(outmsg, *outmsglen, inmsg, size_msghdr);
    cs_memcpy(outmsg + size_msghdr, *outmsglen - size_msghdr, loginres, strlen(loginres) + 1);
    ((struct csmsg_header*)outmsg)->numbytes = htonl(strlen(loginres) + 1);
    *outmsglen = size_msghdr + strlen(loginres) + 1;
}

int s_login_verification(const char* username, const char* password)
{
    return ((strcmp(username, "64bit") == 0) &&
            (strcmp(password, "8-15bytes") == 0));
}

void s_process_communication(char* inmsg, char* outmsg, uint32_t* outmsglen)
{
    uint32_t size_msghdr = sizeof(struct csmsg_header);
    uint32_t msgdatalen = ntohl(GET_HEADER_DATA(inmsg, numbytes));

    cs_memcpy(outmsg, *outmsglen, inmsg, size_msghdr + msgdatalen);
    *outmsglen = size_msghdr + msgdatalen;
    cssleep(200);
}
