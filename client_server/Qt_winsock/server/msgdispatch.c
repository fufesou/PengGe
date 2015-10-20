/**
 * @file msgdispatch.c
 * @brief 
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-18
 */

#include  <stdio.h>
#include  <string.h>
#include  <winsock2.h>
#include   "msgdispatch.h"


#ifdef __cplusplus
extern "C" {
#endif

extern const char* g_loginmsg_header;
extern const char* g_loginmsg_SUCCESS;
extern const char* g_loginmsg_FAIL;
extern const char g_login_delimiter;

static void s_process_login(char* inmsg, char* outmsg, int len_outmsg);
static int s_login_verification(const char *username, const char *password);
static void s_process_communication(char* inmsg, char* outmsg, int len_outmsg);

#ifdef __cplucplus
}
#endif


void process_msg(char* inmsg, char* outmsg, int len_outmsg)
{
    if (strncmp(inmsg, g_loginmsg_header, strlen(g_loginmsg_header)) == 0) {
        s_process_login(inmsg, outmsg, len_outmsg);
    }
    else {
        s_process_communication(inmsg, outmsg, len_outmsg);
    }
}

void s_process_login(char* inmsg, char* outmsg, int len_outmsg)
{
    char* username = NULL;
    char* password = NULL;
    char* delimiter = NULL;
    int loginmsg_header_len;

    loginmsg_header_len = strlen(g_loginmsg_header);
    delimiter = strchr(inmsg + loginmsg_header_len, g_login_delimiter);
    username = inmsg + loginmsg_header_len;
    *delimiter = '\0';
    password = delimiter + 1;

    if (s_login_verification(username, password)) {

        /**
         * @todo of cause we need error handler here.
         */
#ifdef _WIN32
        memcpy_s(outmsg, len_outmsg, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1);
#else
        memcpy(outmsg, g_loginmsg_SUCCESS, strlen(g_loginmsg_SUCCESS) + 1);
#endif
    }
    else  {
         /**
         * @todo of cause we need error handler here.
         */
#ifdef _WIN32
        memcpy_s(outmsg, len_outmsg, g_loginmsg_FAIL, strlen(g_loginmsg_FAIL) + 1);
#else
        memcpy(outmsg, g_loginmsg_FAIL, strlen(g_loginmsg_FAIL) + 1);
#endif
    }
}

int s_login_verification(const char* username, const char* password)
{
    return ((strcmp(username, "64bit") == 0) &&
            (strcmp(password, "8-15bytes") == 0));
}

void s_process_communication(char* inmsg, char* outmsg, int len_outmsg)
{
    /**
    * @todo of cause we need error handler here.
    */
#ifdef _WIN32
    memcpy_s(outmsg, len_outmsg, inmsg, strlen(inmsg) + 1);
#else
    memcpy(outmsg, inmsg, strlen(inmsg) + 1);
#endif
    Sleep(200);
}
