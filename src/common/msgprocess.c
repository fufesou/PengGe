/**
 * @file msgprocess.c
 * @brief  This file defines various message process functions.
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2016-01-06
 * @modified  Wed 2016-01-13 22:55:49 (+0800)
 */

#ifdef WIN32
#include  <winsock2.h>
#else
#include  <netinet/in.h>
#endif

#include  <stdlib.h>
#include  <stdio.h>

#include    "common/config_macros.h"
#include    "common/cstypes.h"
#include    "common/jxiot.h"
#include    "common/list.h"
#include    "common/error.h"
#include    "common/sock_types.h"
#include    "common/utility_wrap.h"
#include    "common/processlist.h"
#include    "common/msgprocess.h"
#include    "common/clearlist.h"
#include    "common/regportlist.h"
#include    "common/msgwrap.h"
#include    "cs/server.h"
#include    "am/account.h"


static LIST_HEAD(s_processlist_client);
static LIST_HEAD(s_processlist_server);
static int s_isInited = 0;

#ifdef __cplusplus
extern "C" {
#endif

static void JXIOT_CALLBACK s_jxprocesslist_clear(void* unused);
static void s_processlist_client_init(void);
static void s_processlist_server_init(void);

#ifdef __cplusplus
}
#endif


void JXIOT_CALLBACK s_jxprocesslist_clear(void* unused)
{
    (void)unused;
    jxprocesslist_clear(&s_processlist_client);
    jxprocesslist_clear(&s_processlist_server);
}

void s_processlist_client_init(void)
{
    jxprocesslist_set(&s_processlist_client, JX_MFLAG_CLIENT_QUERY, jxprocess_client_am, NULL, 0);
}

void s_processlist_server_init(void)
{
    jxprocesslist_set(&s_processlist_server, JX_MFLAG_CLIENT_QUERY, jxprocess_server_am, jxprocess_af_server_am, 0);
    jxprocesslist_set(&s_processlist_server, JX_MFLAG_CLIENT_PORT_REGISTER, jxprocess_server_port, NULL, 0);
}

void jxprocesslist_init(void)
{
    s_isInited = 1;
    s_processlist_client_init();
    s_processlist_server_init();
    jxclearlist_add(s_jxprocesslist_clear, NULL);
}

struct list_head* jxprocesslist_client_default_get(void)
{
    return (s_isInited) ? (&s_processlist_client) : (NULL);
}

struct list_head* jxprocesslist_server_default_get(void)
{
    return (s_isInited) ? (&s_processlist_server) : (NULL);
}


/** @defgroup client process functions
 *
 * @{
 */

int JXIOT_CALLBACK jxprocess_client_am(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    uint32_t id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct jxmsg_header)));
    return am_method_get(id_process)->react(inmsg + sizeof(struct jxmsg_header) + sizeof(uint32_t), outmsg, outmsglen);
}

/** @} */


/** @defgroup server process functions
 *
 * @{
 */

int JXIOT_CALLBACK jxprocess_server_am(char* inmsg, char*outmsg, __jxinout uint32_t* outmsglen)
{
    int ret = 0;
    uint32_t id_process = -1;
    static uint32_t s_fixedlen = sizeof(struct jxmsg_header) + sizeof(uint32_t);
    if (*outmsglen < s_fixedlen)
    {
        return 1;
    }

    jxmemcpy(outmsg, *outmsglen, inmsg, s_fixedlen);
    *outmsglen -= s_fixedlen;

    id_process = ntohl(*(uint32_t*)(inmsg + sizeof(struct jxmsg_header)));
    ret = am_method_get(id_process)->reply(
                inmsg + s_fixedlen,
                &((struct jxmsg_header*)inmsg)->addr,
                (uint32_t)((struct jxmsg_header*)inmsg)->addrlen,
                outmsg + s_fixedlen,
                outmsglen);

    /** The length of variables.
     *
     * - s_fixedlen is only used here as a help variable, and stands for nothing.
     * - *outmsglen is the same as length of total message(without struct jxmsg_header).
     * - 'numbytes' in 'struct jxmsg_header' is the same as *outmsglen.
     */
    *outmsglen += sizeof(uint32_t);
    ((struct jxmsg_header*)outmsg)->numbytes = htonl(*outmsglen);
    return ret;
}

int JXIOT_CALLBACK jxprocess_af_server_am(char* userdata, char* msg)
{
    return jxserver_send(*(jxsock_t*)userdata, msg);
}

int JXIOT_CALLBACK jxprocess_server_port(char* inmsg, char* outmsg, __jxinout uint32_t* outmsglen)
{
    struct jxmsg_header* regportmsg_header = (struct jxmsg_header*)inmsg;

    (void)outmsg;
    (void)outmsglen;

    return jxregportlist_register(
                &((struct sockaddr_in*)(&regportmsg_header->addr))->sin_addr,
                sizeof(struct in_addr),
                ((struct sockaddr_in*)(&regportmsg_header->addr))->sin_port,
                0);
}

/** @} */


