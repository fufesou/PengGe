/**
 * @file msgdispatch.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 * @modified  Wed 2015-11-04 18:53:51 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <sys/socket.h>
#endif

#include  <stdio.h>
#include  <string.h>
#include  <stdint.h>
#include  <semaphore.h>
#include    "lightthread.h"
#include    "sock_types.h"
#include    "utility_wrap.h"
#include    "bufarray.h"
#include    "sendrecv_pool.h"
#include    "msgwrap.h"
#include    "msgdispatch.h"


#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_loginmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static const struct csmsg_header* s_msghdr = NULL;

static void s_process_login(char* inmsg, char* outmsg, int* outmsglen);
static int s_login_verification(const char *username, const char *password);
static void s_process_communication(char* inmsg, char* outmsg, int* outmsglen);

#ifdef __cplucplus
}
#endif


void csserver_process_msg(char* inmsg, char* outmsg, int* outmsglen)
{
    char* msgbegin = inmsg + sizeof(struct csmsg_header);
    s_msghdr = (const struct csmsg_header*)inmsg;

    if (strncmp(msgbegin, g_loginmsg_header, strlen(g_loginmsg_header)) == 0) {
        s_process_login(msgbegin, outmsg, outmsglen);
    }
    else {
        s_process_communication(msgbegin, outmsg, outmsglen);
    }
}

void s_process_login(char* inmsg, char* outmsg, int* outmsglen)
{
    char* delimiter = NULL;
    const char* username = NULL;
    const char* password = NULL;
    const char* loginres = NULL;
    int loginmsg_header_len = 0;
    int size_msghdr = sizeof(struct csmsg_header);

    loginmsg_header_len = strlen(g_loginmsg_header);
    delimiter = strchr(inmsg + loginmsg_header_len, g_login_delimiter);
    username = inmsg + loginmsg_header_len;
    *delimiter = '\0';
    password = delimiter + 1;

    loginres = s_login_verification(username, password) ? g_loginmsg_SUCCESS : g_loginmsg_FAIL;
    cs_memcpy(outmsg, *outmsglen, s_msghdr, size_msghdr);
    cs_memcpy(outmsg + size_msghdr, *outmsglen - size_msghdr, loginres, strlen(loginres) + 1);
    *outmsglen = size_msghdr + strlen(loginres) + 1;
}

int s_login_verification(const char* username, const char* password)
{
    return ((strcmp(username, "64bit") == 0) &&
            (strcmp(password, "8-15bytes") == 0));
}

void s_process_communication(char* inmsg, char* outmsg, int* outmsglen)
{
    int size_msghdr = sizeof(struct csmsg_header);
    cs_memcpy(outmsg, *outmsglen, s_msghdr, size_msghdr);
    cs_memcpy(outmsg + size_msghdr, *outmsglen - size_msghdr, inmsg, s_msghdr->numbytes);
    *outmsglen = size_msghdr + s_msghdr->numbytes;
    cssleep(200);
}
