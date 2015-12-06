/**
 * @file clientcontroller.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Sun 2015-12-06 13:21:14 (+0800)
 */

#include  <QWidget>

#include    "../common/guimacros.h"
#include    "clientcontroller.h"
#include    "loginwindow.h"
#include    "logingwidget.h"
#include    "mainwidget.h"

namespace GuiClient
{

    CController::ELoginStatus CController::m_loginStatus = CController::eLoginInit;

    CController::CController()
        : m_pLoginWidget(new CLoginWindow)
        , m_pLogingWidget(new CLogingWidget)
        , m_pMainWidget(new CMainWidget)
    {
        m_pLoginWidget->hide();
        m_pLogingWidget->hide();
        m_pMainWidget->hide();

        bool bIsLoginConOK = connect(m_pLoginWidget, SIGNAL(login()), this, SLOT(login()));
        Q_ASSERT(bIsLoginConOK);
    }

    CController::~CController()
    {
        SAFE_DEL(m_pLoginWidget);
        SAFE_DEL(m_pLogingWidget);
        SAFE_DEL(m_pMainWidget);
    }

    void CController::showLogin()
    {
        m_pLoginWidget->show();
    }

    void CController::login(const QString& vUserInfo, const QString& vPasswd)
    {
        m_pLoginWidget->hide();
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
