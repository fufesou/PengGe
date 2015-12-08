/**
 * @file clientcontroller.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Tue 2015-12-08 22:44:03 (+0800)
 */

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

static int CS_CALLBACK s_account_create_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static int CS_CALLBACK s_account_verify_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static int CS_CALLBACK s_account_login_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static int CS_CALLBACK s_account_logout_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static int CS_CALLBACK s_account_changeusername_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static int CS_CALLBACK s_account_changepasswd_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);
static int CS_CALLBACK s_account_changegrade_react(char* inmsg, char* outmsg, __inout uint32_t* outmsglen);

#ifdef __cplusplus
}
#endif

namespace GuiClient
{
    CController::ELoginStatus CController::m_loginStatus = CController::eLoginInit;

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

            bool bIsLoginConOK = connect(m_pLoginWidget, SIGNAL(login()), this, SLOT(login()));
            Q_ASSERT(bIsLoginConOK);
        }
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
        csclient_init(m_pCSClient, SOCK_DGRAM, NULL);
        
        m_pServerAddr->sin_family = AF_INET;
        m_pServerAddr->sin_port = htons(vServerPort);
        m_pServerAddr->sin_addr.s_addr = inet_addr(vServerIP);

        csclient_msgpool_dispatch_init(m_pCSClient);

        return true;
    }

    void CController::showLogin(const QString& vUserInfo, const QString& vPasswd)
    {
        m_pLoginWidget->show();
        m_pLogingWidget->show();
    }

    void CController::succeedLogin()
    {
        m_loginStatus = eLoginSucceed;
        m_pLogingWidget->hide();
        m_pMainWidget->show();
    }

    void CController::failLogin()
    {
        m_loginStatus = eLoginFail;
        m_pLogingWidget->hide();
        m_pLoginWidget->show();
    }

    void CController::logout()
    {
        m_loginStatus = eLogout;
        m_pMainWidget->hide();
        m_pLoginWidget->show();
    }

    void CController::exit()
    {
        delete this;
    }
}


/****************************************************
 **                react functions                **
 ***************************************************/
