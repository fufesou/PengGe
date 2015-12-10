/**
 * @file clientcontroller.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Thu 2015-12-10 18:50:14 (+0800)
 */

#include  <QDebug>
#include  <QWidget>

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

#include    "common/cstypes.h"
#include    "common/config_macros.h"
#include    "common/macros.h"
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

#include    "guimacros.h"
#include    "clientcontroller.h"
#include    "loginwindow.h"
#include    "logingwidget.h"
#include    "mainwidget.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char g_succeed;
extern char g_fail;

static int CS_CALLBACK s_account_msg_dispatch(char* unused, char* msg);
static void s_process_create_react(uint32_t msglen, const char* msg);
static void s_process_verify_react(uint32_t msglen, const char* msg);
static void s_process_login_react(uint32_t msglen, const char* msg);
static void s_process_logout_react(uint32_t msglen, const char* msg);
static void s_process_changeusername_react(uint32_t msglen, const char* msg);
static void s_process_changepasswd_react(uint32_t msglen, const char* msg);
static void s_process_changegrade_react(uint32_t msglen, const char* msg);

static struct
{
    const char* methodname;
    void (* process_react)(uint32_t msglen, const char* msg);
} s_namefunc[] = {
    {"account_create", s_process_create_react},
    {"account_verify", s_process_verify_react},
    {"account_login", s_process_login_react},
    {"account_logout", s_process_logout_react},
    {"account_changeusername", s_process_changeusername_react},
    {"account_changepasswd", s_process_changepasswd_react},
    {"account_changegrade", s_process_changegrade_react}
};

#ifdef __cplusplus
}
#endif

static void s_process_create_request(const QString& vUserNumber, const QString& vTel, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_verify_request(const QString& vTel, const QString& vRandCode, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_login_request(const QString& vUserInfo, const QString& vPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_logout_request(struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_changeusername_request(const QString& vPasswd, const QString& vNewUsername, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_changepasswd_request(const QString& vPasswd, const QString& vNewPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_changegrade_request(const QString& vPasswd, uint8_t vGrade, struct csclient* vClient, struct sockaddr_in* vServerAddr);


namespace GuiClient
{
    CController::CController(const char* vServerIP, unsigned short vServerPort)
        : m_pLoginWidget(new CLoginWindow)
        , m_pLogingWidget(new CLogingWidget)
        , m_pMainWidget(new CMainWidget)
        , m_pCSClient(NULL)
        , m_pServerAddr(NULL)
    {
        if (initClient(vServerIP, vServerPort))
        {
            m_pLoginWidget->hide();
            m_pLogingWidget->hide();
            m_pMainWidget->hide();

            bool bIsLoginConOK = connect(
                        m_pLoginWidget,
                        SIGNAL(login(const QString&, const QString&)),
                        this,
                        SLOT(showLogin(const QString&, const QString&)));
            Q_ASSERT(bIsLoginConOK);

            bool bIsLogingStatusConOK = connect(
                        m_pLogingWidget,
                        SIGNAL(loginEnd(const GuiCommon::ELoginStatus&)),
                        this,
                        SLOT(endLogin(const GuiCommon::ELoginStatus&)));
            Q_ASSERT(bIsLogingStatusConOK);
        }

        m_pLoginWidget->show();
    }

    CController::~CController()
    {
        csclearlist_clear();
        free(m_pCSClient);
        free(m_pServerAddr);

        SAFE_DEL(m_pLoginWidget);
        SAFE_DEL(m_pLogingWidget);
        SAFE_DEL(m_pMainWidget);
    }

    bool CController::initClient(const char* vServerIP, unsigned short vServerPort)
    {
        m_pCSClient = (struct csclient*)malloc(sizeof(struct csclient));
        m_pServerAddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

        cssock_envinit();
        csclient_init(m_pCSClient, SOCK_DGRAM);
        
        m_pServerAddr->sin_family = AF_INET;
        m_pServerAddr->sin_port = htons(vServerPort);
        m_pServerAddr->sin_addr.s_addr = inet_addr(vServerIP);

        csclient_msgpool_dispatch_init(NULL, s_account_msg_dispatch);

        return true;
    }

    void CController::showLogin(const QString& vUserInfo, const QString& vPasswd)
    {
        s_process_login_request(vUserInfo, vPasswd, m_pCSClient, m_pServerAddr);
        m_pLoginWidget->show();
        m_pLoginWidget->hide();

        m_pLogingWidget->show();
        dynamic_cast<CLogingWidget*>(m_pLogingWidget)->beginLogin();
    }

    void CController::endLogin(const GuiCommon::ELoginStatus& vStatus)
    {
        m_pLogingWidget->hide();

        if (vStatus == GuiCommon::eTimeout)
        {
            qDebug() << "login: timeout";
            m_pLoginWidget->show();
        }
        else if (vStatus == GuiCommon::eFail)
        {
            qDebug() << "login: error user info or passwd";
            m_pLogingWidget->show();
        } else if (vStatus == GuiCommon::eSucceed)
        {
            m_pMainWidget->show();
        }
    }

    void CController::logout()
    {
        m_pMainWidget->hide();
        m_pLoginWidget->show();
    }

    void CController::exit()
    {
        delete this;
    }
}


/*********************************************************
 **                 request client msg                  **
 ********************************************************/
void s_process_create_request(const QString& vUserNumber, const QString& vTel, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{

}

void s_process_verify_request(const QString& vTel, const QString& vRandCode, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{

}

void s_process_login_request(const QString& vUserInfo, const QString& vPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_login_request(vUserInfo.toUtf8().constData(),
                                 vPasswd.toUtf8().constData(),
                                 vClient->sendbuf,
                                 &vClient->len_senddata) == 0) {
    } else {
        csclient_udp_once(vClient, (struct sockaddr*)vServerAddr, sizeof(*vServerAddr));
    }
}

void s_process_logout_request(struct csclient* vClient, struct sockaddr_in* vServerAddr)
{

}

void s_process_changeusername_request(const QString& vPasswd, const QString& vNewUsername, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{

}

void s_process_changepasswd_request(const QString& vPasswd, const QString& vNewPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{

}

void s_process_changegrade_request(const QString& vPasswd, uint8_t vGrade, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
}

/*********************************************************
 **                  react client msg                   **
 ********************************************************/
int s_account_msg_dispatch(char* unused, char* msg)
{
    int i = 0;
    int num_funcs = NUM_ARR(s_namefunc);
    uint32_t namelen = 0;

    (unused);

    for (; i<num_funcs; ++i) {
        namelen = strlen(s_namefunc[i].methodname);
        if (memcmp(msg, s_namefunc[i].methodname, namelen)) {
            s_namefunc[i].process_react(*(uint32_t*)(msg + namelen), msg + namelen + sizeof(uint32_t));
        }
    }

    return 0;
}

void s_process_create_react(uint32_t msglen, const char* msg)
{
    (void)msglen;
    if (msg[0] == g_succeed) {
    } else if (msg[0] == g_fail) {
    } else {
    }
}

void s_process_verify_react(uint32_t msglen, const char* msg)
{
    (void)msglen;
    if (msg[0] == g_succeed) {
    } else if (msg[0] == g_fail) {
    } else {
    }
}

void s_process_login_react(uint32_t msglen, const char* msg)
{
    (void)msglen;
    if (msg[0] == g_succeed) {
        GuiClient::CLogingWidget::setLoginStatus(GuiCommon::eSucceed);
    } else if (msg[0] == g_fail) {
        GuiClient::CLogingWidget::setLoginStatus(GuiCommon::eFail);
    } else {
        qDebug() << "login: unkown message.";
    }
}

void s_process_logout_react(uint32_t msglen, const char* msg)
{
}

void s_process_changeusername_react(uint32_t msglen, const char* msg)
{
}

void s_process_changepasswd_react(uint32_t msglen, const char* msg)
{
}

void s_process_changegrade_react(uint32_t msglen, const char* msg)
{
}

