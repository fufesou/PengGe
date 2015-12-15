/**
 * @file requestingwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-11
 * @modified  Tue 2015-12-15 19:39:53 (+0800)
 */

#ifndef _REQUESTINGWIDGET_H
#define _REQUESTINGWIDGET_H

#include  <QWidget>

#include  "guitypes.h"

QT_FORWARD_DECLARE_CLASS(QTimer)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QProgressBar)

namespace GuiClient
{
    class CRequestingWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CRequestingWidget(QWidget* vParent = 0, int vTimeoutMsec = 30*1000, int vIntervalMsec = 300);
        virtual ~CRequestingWidget() {}

        static void setRequestStatus(GuiCommon::ERequestStatus vStatus) { s_requestStatus = vStatus; }

        void beginRequest(void);

    signals:
        void requestEnd(const GuiCommon::ERequestStatus& vStatus);

    private:
        void initWidget(void);
        void initTimer(void);

    private slots:
        void updateProgressBar(void);

    private:
        int m_timeoutMsec;
        int m_elapsedTimes;
        int m_interval;
        QTimer* m_pTimer;
        QLabel* m_plbScene;
        QProgressBar* m_ppbProgress;

        static GuiCommon::ERequestStatus s_requestStatus;
    };
}

#endif //REQUESTINGWIDGET_H
