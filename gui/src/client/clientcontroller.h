/**
 * @file clientcontroller.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Tue 2015-12-08 00:56:22 (+0800)
 */

#ifndef _CLIENTCONTROLLER_H
#define _CLIENTCONTROLLER_H

#include  <QObject>

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

        void showLogin(const QString& vUserInfo, const QString& vPasswd);
        static ELoginStatus getLoginStatus() { return m_loginStatus; }

    public slots:
        void succeedLogin(void);
        void failLogin(void);
        void logout(void);
        void exit(void);

    private:
        bool initClient(const char* vServerIP, unsigned short vServerPort);

    private:
        static ELoginStatus m_loginStatus;

        QWidget* m_pLoginWidget;
        QWidget* m_pLogingWidget;
        QWidget* m_pMainWidget;

        struct csclient* m_pCSClient;
        struct sockaddr_in* m_pServerAddr;
    };
}

#endif //CLIENTCONTROLLER_H
