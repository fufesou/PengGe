/**
 * @file clientcontroller.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Wed 2015-12-16 20:44:22 (+0800)
 */

#ifndef _CLIENTCONTROLLER_H
#define _CLIENTCONTROLLER_H

#include  <QObject>
#include  <QMap>

#include    "guitypes.h"

QT_FORWARD_DECLARE_STRUCT(jxclient)
QT_FORWARD_DECLARE_CLASS(QWidget)

namespace GuiCommon
{
    QT_FORWARD_DECLARE_CLASS(CGuiThread)
}

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
        void showLogin(void);
        void showRegister(void);
        void showVerify(void);
        void showLogining(const QString& vUserInfo, const QString& vPasswd);
        void showRegistering(const QString& vUserNum, const QString& vTelNum);
        void showVerifying(const QString& vTelNum, const QString& vRandCode);

    private:
        void showWidget(const QString& vSigWidget);
        void initWidgets(void);
        void insertWidgets(void);
        void hideAllWidgets(void);
        bool initClient(const char* vServerIP, unsigned short vServerPort);

    private slots:
        void endLogining(const GuiCommon::ERequestStatus& vStatus);
        void endRegistering(const GuiCommon::ERequestStatus& vStatus);
        void endVerifying(const GuiCommon::ERequestStatus& vStatus);

    private:
        QMap<QString, QWidget*> m_mapWidget;

        GuiCommon::CGuiThread* m_pSendThread;
        struct jxclient* m_pjxclient;
        struct sockaddr_in* m_pServerAddr;
    };
}

#endif //CLIENTCONTROLLER_H
