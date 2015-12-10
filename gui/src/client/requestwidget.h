/**
 * @file requestwidget.h
 * @brief  CRequestWidget is the base widget for processing request(register, verify, login, etc).
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-10
 * @modified  Fri 2015-12-11 00:13:36 (+0800)
 */

#ifndef _REQUESTWIDGET_H
#define _REQUESTWIDGET_H

#include  <QWidget>

#include    "guitypes.h"

QT_FORWARD_DELCARE_CLASS(QTimer)
QT_FORWARD_DELCARE_CLASS(QLineEdit)
QT_FORWARD_DELCARE_CLASS(QPushButton)

namespace GuiClient
{
    class CRequestWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CRequestWidget(int vTimeoutMsec = 30*1000, int vIntervalMsec = 300, QWidget* vParent = 0);
        virtual ~CRequestWidget() {}

        static void setRequestStatus(GuiCommon::ERequestStatus vStatus) { s_requestStatus = vStatus; }

        void setLineEditFirst(QLineEdit* vLE);
        void setLineEditSecond(QLineEdit* vLE);

    signals:
        void requestEnd(const GuiCommon::ERequestStatus& vStatus);

    private:
        void initWidget(void);
        void initTimer(void);

    private slots:
        void beginRequest(void);
        void updateProgressBar(void);

    private:
        int m_timeoutMsec;
        int m_elapsedTimes;
        int m_interval;
        QTimer* m_pTimer;

        QLineEdit* m_pleFirstInfo;
        QLineEdit* m_pleSecondInfo;

        QPushButton* m_pbtnRequest;

        static GuiCommon::ERequestStatus s_requestStatus;
    };
}


#endif //REQUESTWIDGET_H
