/**
 * @file clientcontroller.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Thu 2015-12-10 18:36:51 (+0800)
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

    private:
        bool initClient(const char* vServerIP, unsigned short vServerPort);

    private slots:
        void endLogin(const GuiCommon::ELoginStatus& vStatus);

    private:
        QWidget* m_pLoginWidget;
        QWidget* m_pLogingWidget;
        QWidget* m_pMainWidget;

        struct csclient* m_pCSClient;
        struct sockaddr_in* m_pServerAddr;
    };
}

#endif //CLIENTCONTROLLER_H
