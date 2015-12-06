/**
 * @file clientcontroller.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Sun 2015-12-06 13:20:28 (+0800)
 */

#ifndef _CLIENTCONTROLLER_H
#define _CLIENTCONTROLLER_H

#include  <QObject>

QT_FORWARD_DECLARE_CLASS(QWidget)

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
        CController(void);
        ~CController();

        void showLogin(const QString& vUserInfo, const QString& vPasswd);
        static ELoginStatus getLoginStatus() { return m_loginStatus; }

    public slots:
        void login(void);
        void succeedLogin(void);
        void failLogin(void);
        void logout(void);
        void exit(void);

    private:
        static ELoginStatus m_loginStatus;

        QWidget* m_pLoginWidget;
        QWidget* m_pLogingWidget;
        QWidget* m_pMainWidget;
    };
}


#endif //CLIENTCONTROLLER_H
