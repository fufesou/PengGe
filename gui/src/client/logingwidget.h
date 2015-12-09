/**
 * @file logingwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Wed 2015-12-09 23:27:51 (+0800)
 */

#include  <QWidget>

#ifndef _LOGINGWIDGET_H
#define _LOGINGWIDGET_H

#include  "guitypes.h"

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QProgressBar)

namespace GuiClient
{
    class CLogingWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CLogingWidget(int vTimeoutMsec = 10*1000, QWidget* vParent = 0);

        static void setLoginStatus(GuiCommon::ELoginStatus vStatus) { s_loginStatus = vStatus; }

        void beginLogin(void);

    signals:
        void cancelLogin(void);
        void loginEnd(const GuiCommon::ELoginStatus& voStatus);

    private:
        void initWidget(void);

    private:
        int m_timeoutSec;
        QLabel* m_plbScene;
        QPushButton* m_pbtnCancel;
        QProgressBar* m_ppbProgress;

        static GuiCommon::ELoginStatus s_loginStatus;
    };
}


#endif //LOGINGWIDGET_H
