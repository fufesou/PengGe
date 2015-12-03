/**
 * @file clientcontroller.h
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-12-03
 * @modified  周四 2015-12-03 22:21:15 中国标准时间
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

        void showLogin(void);
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
