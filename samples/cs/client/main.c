/**
 * @file main.c
 * @brief Client must login before exchange message with server. 
 * The format of login message is "login: <username> <password>".
 * The format of logout message is "logout".
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-10-01
 * @modified  Thu 2015-11-19 19:29:42 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#include  <windows.h>
#else
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#endif

#include  <stdio.h>
#include  <stdlib.h>
#include  <stdint.h>
#include    "config_macros.h"
#include    "macros.h"
#include    "sock_types.h"
#include    "sock_wrap.h"
#include    "utility_wrap.h"
#include    "client.h"


void client_udp_init(struct csclient* cli);
void client_udp_clear(void);
int conv_cmd_msg(char* cmd, char* msg, uint32_t size_msg);

int main(int argc, char* argv[])
{
    struct csclient udpclient;
    struct sockaddr_in serveraddr;
	char data_input[1024];
	FILE* fp_input = NULL;

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


#define TEST_FILE_INPUT

#ifdef TEST_FILE_INPUT
    client_udp_init(&udpclient);
	while (!feof(fp_input)) {
		fgets(data_input, sizeof(data_input), fp_input);
		if (conv_cmd_msg(data_input, udpclient.sendbuf, sizeof(udpclient.sendbuf)) != 0) {
            csclient_udp_once(&udpclient, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
		}
	}
    client_udp_clear();
#else
    csclient_udp(&udpclient, stdin, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
#endif

#undef TEST_FILE_INPUT

	cssock_envclear();

    return 0;
}

