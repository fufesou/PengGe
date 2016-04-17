/**
 * @file main.c
 * @brief
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-09-25
 * @modifiedWed 2016-01-13 22:58:19 (+0800)
 */

#ifndef CMAKE_PRO
#ifdef _MSC_VER 
//#pragma comment(lib, "../../Debug/pgcsd.lib")
#pragma comment(lib, "D:/Projects/PengGe/samples/Debug/pgcsd.lib")
#endif
#endif

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
#include    "common/config_macros.h"
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/sock_types.h"
#include    "common/sock_wrap.h"
#include    "common/list.h"
#include    "common/clearlist.h"
#include    "common/processlist.h"
#include    "common/msgprocess.h"
#include    "cs/server.h"
#include    "am/server_account.h"


void s_check_args(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    /** the server object must live to the end of process. */
    struct jxserver udpserver;

    s_check_args(argc, argv);

    printf("use the cs version of %d.%d", JXIOT_CS_VERSION_MAJOR, JXIOT_CS_VERSION_MINOR);

    jxsock_envinit();
    if (am_server_account_init() != 0) {
        fprintf(stderr, "server: init account fail.\n");
        jxclearlist_clear();
        return 1;
    }

    jxserver_init(&udpserver, SOCK_DGRAM, atoi(argv[1]), htonl(INADDR_ANY));
    jxsock_print(udpserver.hsock, udpserver.prompt);

    jxprocesslist_init();
    jxserver_msgpool_dispatch_init(&udpserver, jxprocesslist_server_default_get());

    jxserver_udp(&udpserver);

    jxclearlist_clear();

    return 0;
}

void s_check_args(int argc, char* argv[])
{
    if (argc < 2) {
        printf("usage: server <port>.");
        exit(1);
    }
    (void)(argv);
}

