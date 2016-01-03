/**
 * @file main.c
 * @brief Client must login before exchange message with server. 
 * The format of login message is "login: <username> <password>".
 * The format of logout message is "logout".
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-01
 * @modified  Sun 2015-12-06 15:14:15 (+0800)
 */

#ifdef _MSC_VER 
#pragma comment(lib, "../../../build/lib/pgcsd.lib")
#endif

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#endif

#ifndef _MSC_VER  /* *nix */
#include  <semaphore.h>
#endif

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/jxiot.h"
#include    "common/list.h"
#include    "common/lightthread.h"
#include    "common/bufarray.h"
#include    "common/sock_types.h"
#include    "common/sock_wrap.h"
#include    "common/utility_wrap.h"
#include    "common/clearlist.h"
#include    "cs/msgpool.h"
#include    "cs/msgpool_dispatch.h"
#include    "cs/client.h"
#include    "am/client_account.h"

int s_msgdispatch(const char* inmsg, char* outmsg, uint32_t* outmsglen);

int main(int argc, char* argv[])
{
    struct csclient udpclient;
    struct sockaddr_in serveraddr;
	FILE* fp_input = NULL;
	char data_input[1024];

    if (argc < 3) {
        printf("usage: client <server ip> <port>.");
        exit(1);
    }

    if (cs_fopen(&fp_input, "../client/input_test.txt", "r") != 0) {
        fprintf(stderr, "cannot open ../client/input_test.txt.\n");
		exit(1);
	}

	cssock_envinit();
    csclient_init(&udpclient, SOCK_DGRAM);

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons((unsigned short)atoi(argv[2]));
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);


    csclient_msgpool_dispatch_init(NULL, NULL);
#define TEST_FILE_INPUT

#ifdef TEST_FILE_INPUT
    csclient_connect(&udpclient, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    while (!feof(fp_input)) {
        udpclient.len_senddata = udpclient.size_senbuf;
        data_input[0] = 0;
        fgets(data_input, sizeof(data_input), fp_input);
        if (s_msgdispatch(data_input, udpclient.sendbuf, &udpclient.len_senddata) != 0) {
			fprintf(stderr, "dispatch message error.\n");
		} else {
			csclient_udp_once(&udpclient, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
		}
	}
#else
    csclient_udp(&udpclient, stdin, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
#endif

#undef TEST_FILE_INPUT

    csclearlist_clear();

#ifdef _MSC_VER
	system("pause");
#endif

    return 0;
}


int s_msgdispatch(const char* inmsg, char* outmsg, uint32_t* outmsglen)
{
	switch (*inmsg) {
		case '0':
            return am_account_create_request(inmsg + 1, strchr(inmsg + 1, '\0') + 1, outmsg, outmsglen);

		case '1':
            return am_account_verify_request(inmsg + 1, strchr(inmsg + 1, '\0') + 1, outmsg, outmsglen);

		case '2':
            return am_account_login_request(inmsg + 1, strchr(inmsg + 1, '\0') + 1, outmsg, outmsglen);

        case '3':
            return am_account_logout_request(outmsg, outmsglen);

        case '4':
            return am_account_changeusername_request(inmsg + 1, strchr(inmsg + 1, '\0') + 1, outmsg, outmsglen);

        case '5':
            return am_account_changepasswd_request(inmsg + 1, strchr(inmsg+1, '\0') + 1, outmsg, outmsglen);
	}

	return 1;
}
