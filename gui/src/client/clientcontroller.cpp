/**
 * @file clientcontroller.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Wed 2015-12-16 20:45:47 (+0800)
 */

#include  <QDebug>
#include  <QLabel>
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
#include    "guithread.h"
#include    "clientcontroller.h"
#include    "loginwidget.h"
#include    "loginingwidget.h"
#include    "registerwidget.h"
#include    "registeringwidget.h"
#include    "verifywidget.h"
#include    "verifyingwidget.h"
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

static void s_process_create_request(QString vUserNumber, QString vTel, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_verify_request(QString vTel, QString vRandCode, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_login_request(QString vUserInfo, QString vPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_logout_request(struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_changeusername_request(QString vPasswd, QString vNewUsername, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_changepasswd_request(QString vPasswd, QString vNewPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr);
static void s_process_changegrade_request(QString vPasswd, uint8_t vGrade, struct csclient* vClient, struct sockaddr_in* vServerAddr);


namespace GuiClient
{
    static QString s_sigMainWidget("MainWidget");
    static QString s_sigLoginWidget("LoginWidget");
    static QString s_sigLoginingWidget("LoginingWidget");
    static QString s_sigRegisterWidget("RegisterWidget");
    static QString s_sigRegisteringWidget("RegisteringWidget");
    static QString s_sigVerifyWidget("VerifyWidget");
    static QString s_sigVerifyingWidget("VerifyingWidget");

    CController::CController(const char* vServerIP, unsigned short vServerPort)
        : m_pSendThread(new GuiCommon::CGuiThread(this))
        , m_pCSClient(NULL)
        , m_pServerAddr(NULL)
    {
        if (initClient(vServerIP, vServerPort))
        {
            m_pSendThread->setFixedArgs(m_pCSClient, m_pServerAddr);
            initWidgets();
        }
        else
        {
            Q_ASSERT(0);
        }
    }

    CController::~CController()
    {
        csclearlist_clear();
        free(m_pCSClient);
        free(m_pServerAddr);

        foreach (QWidget* pWidget, m_mapWidget) {
            SAFE_DEL(pWidget);
        }
    }

    void CController::initWidgets()
    {
        insertWidgets();
        hideAllWidgets();
        showWidget(s_sigLoginWidget);
    }

    void CController::insertWidgets()
    {
        QWidget* pMainWidget = new CMainWidget();
        m_mapWidget.insert(s_sigMainWidget, pMainWidget);
        bool VARIABLE_IS_NOT_USED bIsLogoutOK = connect(
                    pMainWidget,
                    SIGNAL(logout()),
                    this,
                    SLOT(logout()));
        Q_ASSERT(bIsLogoutOK);

        QWidget* pLoginWidget = new CLoginWidget();
        bool VARIABLE_IS_NOT_USED bIsLoginConOK = connect(
                                pLoginWidget,
                                SIGNAL(request(const QString&, const QString&)),
                                this,
                                SLOT(showLogining(const QString&, const QString&)));
        Q_ASSERT(bIsLoginConOK);
        bool VARIABLE_IS_NOT_USED bIsLogin2RegisterConOK = connect(
                                pLoginWidget,
                                SIGNAL(switch2Register()),
                                this,
                                SLOT(showRegister()));
        Q_ASSERT(bIsLogin2RegisterConOK);
        m_mapWidget.insert(s_sigLoginWidget, pLoginWidget);

        QWidget* pLoginingWidget = new CLoginingWidget();
        bool VARIABLE_IS_NOT_USED bIsLogingStatusConOK = connect(
                                pLoginingWidget,
                                SIGNAL(requestEnd(const GuiCommon::ERequestStatus&)),
                                this,
                                SLOT(endLogining(const GuiCommon::ERequestStatus&)));
        Q_ASSERT(bIsLogingStatusConOK);
        m_mapWidget.insert(s_sigLoginingWidget, pLoginingWidget);

        QWidget* pRegisterWidget = new CRegisterWidget();
        bool VARIABLE_IS_NOT_USED bIsRegisterConOK = connect(
                    pRegisterWidget,
                    SIGNAL(request(const QString&, const QString&)),
                    this,
                    SLOT(showRegistering(const QString&, const QString&)));
        Q_ASSERT(bIsRegisterConOK);
        bool VARIABLE_IS_NOT_USED bIsRegister2LoginConOK = connect(
                    pRegisterWidget,
                    SIGNAL(switch2Login()),
                    this,
                    SLOT(showLogin()));
        Q_ASSERT(bIsRegister2LoginConOK);
        bool VARIABLE_IS_NOT_USED bIsRegister2VerifyConOK = connect(
                    pRegisterWidget,
                    SIGNAL(switch2Verify()),
                    this,
                    SLOT(showVerify()));
        Q_ASSERT(bIsRegister2VerifyConOK);
        m_mapWidget.insert(s_sigRegisterWidget, pRegisterWidget);

        QWidget* pRegisteringWidget = new CRegisteringWidget();
        bool VARIABLE_IS_NOT_USED bIsReigsteringConOK = connect(
                    pRegisteringWidget,
                    SIGNAL(requestEnd(const GuiCommon::ERequestStatus&)),
                    this,
                    SLOT(endRegistering(const GuiCommon::ERequestStatus&)));
        Q_ASSERT(bIsReigsteringConOK);
        m_mapWidget.insert(s_sigRegisteringWidget, pRegisteringWidget);

        QWidget* pVerifyWidget = new CVerifyWidget();
                bool VARIABLE_IS_NOT_USED bIsVerifyConOK = connect(
                    pVerifyWidget,
                    SIGNAL(request(const QString&, const QString&)),
                    this,
                    SLOT(showVerifying(const QString&, const QString&)));
        Q_ASSERT(bIsVerifyConOK);
        bool VARIABLE_IS_NOT_USED bIsVerify2Login = connect(
                    pVerifyWidget,
                    SIGNAL(switch2Login()),
                    this,
                    SLOT(showLogin()));
        Q_ASSERT(bIsVerify2Login);
        m_mapWidget.insert(s_sigVerifyWidget, pVerifyWidget);

        QWidget* pVerifyingWidget = new CVerifyingWidget();
                bool VARIABLE_IS_NOT_USED bIsVerifyingConOK = connect(
                    pVerifyingWidget,
                    SIGNAL(requestEnd(const GuiCommon::ERequestStatus&)),
                    this,
                    SLOT(endVerifying(const GuiCommon::ERequestStatus&)));
        Q_ASSERT(bIsVerifyingConOK);
        m_mapWidget.insert(s_sigVerifyingWidget, pVerifyingWidget);
    }

    void CController::hideAllWidgets()
    {
        foreach (QWidget* pWidget, m_mapWidget) {
            pWidget->hide();
        }
    }

    void CController::showWidget(const QString& vSigWidget)
    {
        m_mapWidget.value(vSigWidget)->show();
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

#define SHOW_DEF(sig) \
    void CController::show##sig() \
    { \
        hideAllWidgets(); \
        showWidget(s_sig##sig##Widget); \
    }

    SHOW_DEF(Login)
    SHOW_DEF(Register)
    SHOW_DEF(Verify)

#undef SHOW_DEF

    void CController::showLogining(const QString& vUserInfo, const QString& vPasswd)
    {
        hideAllWidgets();
        showWidget(s_sigLoginingWidget);
        // s_process_login_request(vUserInfo, vPasswd, m_pCSClient, m_pServerAddr);
        m_pSendThread->setArgs((void*)s_process_login_request, vUserInfo, vPasswd);
        m_pSendThread->start();
        dynamic_cast<CLoginingWidget*>(m_mapWidget[s_sigLoginingWidget])->beginRequest();
    }

    void CController::showRegistering(const QString& vUserNum, const QString& vTelNum)
    {
        hideAllWidgets();
        showWidget(s_sigRegisteringWidget);
        // s_process_create_request(vUserNum, vTelNum, m_pCSClient, m_pServerAddr);
        m_pSendThread->setArgs((void*)s_process_create_request, vUserNum, vTelNum);
        m_pSendThread->start();
        dynamic_cast<CRegisteringWidget*>(m_mapWidget[s_sigRegisteringWidget])->beginRequest();
    }

    void CController::showVerifying(const QString& vTelNum, const QString& vRandCode)
    {
        hideAllWidgets();
        showWidget(s_sigVerifyingWidget);
        // s_process_verify_request(vTelNum, vRandCode, m_pCSClient, m_pServerAddr);
        m_pSendThread->setArgs((void*)s_process_verify_request, vTelNum, vRandCode);
        m_pSendThread->start();
        dynamic_cast<CVerifyingWidget*>(m_mapWidget[s_sigVerifyingWidget])->beginRequest();
    }

    void CController::endLogining(const GuiCommon::ERequestStatus& vStatus)
    {
        hideAllWidgets();

        if (vStatus == GuiCommon::eTimeout)
        {
            qDebug() << "login: timeout";
            showWidget(s_sigLoginWidget);
        }
        else if (vStatus == GuiCommon::eFail)
        {
            qDebug() << "login: error user info or passwd";
            showWidget(s_sigLoginWidget);
        } else if (vStatus == GuiCommon::eSucceed)
        {
            showWidget(s_sigMainWidget);
        }
    }

    void CController::endRegistering(const GuiCommon::ERequestStatus& vStatus)
    {
        hideAllWidgets();

        if (vStatus == GuiCommon::eTimeout)
        {
            qDebug() << "register: timeout";
            showWidget(s_sigRegisterWidget);
        }
        else if (vStatus == GuiCommon::eFail)
        {
            qDebug() << "register: fail";
            showWidget(s_sigRegisterWidget);
        } else if (vStatus == GuiCommon::eSucceed) {
            showWidget(s_sigVerifyWidget);
        }
    }

    void CController::endVerifying(const GuiCommon::ERequestStatus& vStatus)
    {
        hideAllWidgets();

        if (vStatus == GuiCommon::eTimeout)
        {
            qDebug() << "verify: timeout";
            showWidget(s_sigVerifyWidget);
        }
        else if (vStatus == GuiCommon::eFail)
        {
            qDebug() << "verify: error user info or passwd";
            showWidget(s_sigVerifyWidget);
        } else if (vStatus == GuiCommon::eSucceed) {
            showWidget(s_sigMainWidget);
        }
    }

    void CController::logout()
    {
        s_process_logout_request(m_pCSClient, m_pServerAddr);
        hideAllWidgets();
        showWidget(s_sigLoginWidget);
    }

    void CController::exit()
    {
        delete this;
    }
}


/*********************************************************
 **                 request client msg                  **
 ********************************************************/
#define SEND_COMMON \
    qDebug() << "send request begin."; \
    csclient_udp_once(vClient, (struct sockaddr*)vServerAddr, sizeof(*vServerAddr)); \
    qDebug() << "send request end."; \
    return;

void s_process_create_request(QString vUserNumber, QString vTel, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_create_request(vUserNumber.toLatin1().constData(),
                                 vTel.toLatin1().constData(),
                                 vClient->sendbuf,
                                 &vClient->len_senddata) == 0) {
        SEND_COMMON
    }
    qDebug() << "cannot handle create request";
}

void s_process_verify_request(QString vTel, QString vRandCode, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_verify_request(vTel.toLatin1().constData(),
                                 vRandCode.toLatin1().constData(),
                                 vClient->sendbuf,
                                 &vClient->len_senddata) == 0) {
        SEND_COMMON
    }
    qDebug () << "cannot handle verify request";
}

void s_process_login_request(QString vUserInfo, QString vPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_login_request(vUserInfo.toLatin1().constData(),
                                 vPasswd.toLatin1().constData(),
                                 vClient->sendbuf,
                                 &vClient->len_senddata) == 0) {
        SEND_COMMON
    }
    qDebug() << "cannot handle login request";
}

void s_process_logout_request(struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_logout_request(vClient->sendbuf, &vClient->len_senddata) == 0) {
        SEND_COMMON
    }
    qDebug() << "cannot handle logout request";
}

void s_process_changeusername_request(QString vPasswd, QString vNewUsername, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_changeusername_request(vPasswd.toLatin1().data(),
                                          vNewUsername.toLatin1().data(),
                                          vClient->sendbuf,
                                          &vClient->len_senddata) == 0) {
        SEND_COMMON
    }
    qDebug() << "cannot handle logout request";
}

void s_process_changepasswd_request(QString vPasswd, QString vNewPasswd, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_changepasswd_request(vPasswd.toLatin1().data(),
                                        vNewPasswd.toLatin1().data(),
                                        vClient->sendbuf,
                                        &vClient->len_senddata) == 0) {
        SEND_COMMON
    }
    qDebug() << "cannot handle logout request";
}

void s_process_changegrade_request(QString vPasswd, uint8_t vGrade, struct csclient* vClient, struct sockaddr_in* vServerAddr)
{
    vClient->len_senddata = vClient->size_senbuf;
    if (am_account_changegrade_request(vPasswd.toLatin1().data(),
                                       vGrade,
                                       vClient->sendbuf,
                                       &vClient->len_senddata) == 0) {
        SEND_COMMON
    }
    qDebug() << "cannot handle logout request";
}
#undef SEND_COMMON

/*********************************************************
 **                  react client msg                   **
 ********************************************************/
int s_account_msg_dispatch(char* unused, char* msg)
{
    int i = 0;
    int num_funcs = NUM_ARR(s_namefunc);
    uint32_t namelen = 0;

    (void)unused;

    for (; i<num_funcs; ++i) {
        namelen = strlen(s_namefunc[i].methodname);
        if (memcmp(msg, s_namefunc[i].methodname, namelen) == 0) {
            s_namefunc[i].process_react(*(uint32_t*)(msg + namelen), msg + namelen + sizeof(uint32_t));
        }
    }

    return 0;
}

void s_process_create_react(uint32_t msglen, const char* msg)
{
    (void)msglen;
    if (msg[0] == g_succeed) {
        GuiClient::CLoginingWidget::setRequestStatus(GuiCommon::eSucceed);
    } else if (msg[0] == g_fail) {
        GuiClient::CLoginingWidget::setRequestStatus(GuiCommon::eFail);
    } else {
        qDebug() << "react: unkown message.";
    }
}

void s_process_verify_react(uint32_t msglen, const char* msg)
{
    s_process_create_react(msglen, msg);
}

void s_process_login_react(uint32_t msglen, const char* msg)
{
    s_process_create_react(msglen, msg);
}

void s_process_logout_react(uint32_t msglen, const char* msg)
{
    s_process_create_react(msglen, msg);
}

void s_process_changeusername_react(uint32_t msglen, const char* msg)
{
    s_process_create_react(msglen, msg);
}

void s_process_changepasswd_react(uint32_t msglen, const char* msg)
{
    s_process_create_react(msglen, msg);
}

void s_process_changegrade_react(uint32_t msglen, const char* msg)
{
    s_process_create_react(msglen, msg);
}
