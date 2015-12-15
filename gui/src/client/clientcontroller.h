/**
 * @file clientcontroller.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Tue 2015-12-15 19:35:46 (+0800)
 */

#ifndef _CLIENTCONTROLLER_H
#define _CLIENTCONTROLLER_H

#include  <QObject>

#include    "guitypes.h"

QT_FORWARD_DECLARE_CLASS(QWidget)

QT_FORWARD_DECLARE_STRUCT(csclient)

namespace GuiClient
{
    class CController : public QObject
    {
        Q_OBJECT

    public:
        enum ELoginStatus
        {
            eLoginInit,
            eLoginSucceed,
            eLoginFail,
            eLogout
        };

    public:
        CController(const char* vServerIP, unsigned short vServerPort);
        ~CController();

    public slots:
        void logout(void);
        void exit(void);
        void showLogin(const QString& vUserInfo, const QString& vPasswd);
        void showRegister(const QString& vUserNum, const QString& vTelNum);
        void showVerify(const QString& vTelNum, const QString& vRandCode);

    private:
        bool initClient(const char* vServerIP, unsigned short vServerPort);

    private slots:
        void endLogin(const GuiCommon::ERequestStatus& vStatus);
        void endRegister(const GuiCommon::ERequestStatus& vStatus);
        void endVerify(const GuiCommon::ERequestStatus& vStatus);

    private:
        QWidget* m_pLoginWidget;
        QWidget* m_pLoginingWidget;
        QWidget* m_pRegisterWidget;
        QWidget* m_pRegisteringWidget;
        QWidget* m_pVerifyWidget;
        QWidget* m_pVerifyingWidget;
        QWidget* m_pMainWidget;

        struct csclient* m_pCSClient;
        struct sockaddr_in* m_pServerAddr;
    };
}

#endif //CLIENTCONTROLLER_H
