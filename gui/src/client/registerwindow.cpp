/**
 * @file registerwindow.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-11
 * @modified  Fri 2015-12-11 00:15:55 (+0800)
 */

#include  <QLabel>
#include  <QLineEdit>
#include  <QPushButton>
#include  <QVBoxLayout>

#include    "registerwindow.h"

namespace GuiClient
{
    CRegisterWindow::CRegisterWindow(int vTimeoutMsec, int vIntervalMsec, QWidget* vParent)
        : QWidget(vParent)
        , m_timeoutMsec(vTimeoutMsec)
        , m_elapsedTimes(0)
        , m_interval(vIntervalMsec)
        , m_pTimer(NULL)
        , m_pleFirstInfo(NULL)
        , m_pleSecondInfo(NULL)
    {
        initWidget();
        initTimer();

        setGeometry(200, 200, 500, 400);
        setFixedSize(500, 400);
    }

    void CRegisterWindow::setLineEditFirst(QLineEdit* vLE)
    {
        if (m_pleFirstInfo != NULL)
        {
            delete m_pleFirstInfo;
        }
        m_pleFirstInfo = vLE;
    }

    void CRegisterWindow::setLineEditSecond(QLineEdit* vLE)
    {
        if (m_pleSecondInfo != NULL)
        {
            delete m_pleSecondInfo;
        }
        m_pleSecondInfo = vLE;
    }

    void CRegisterWindow::initWidget()
    {
        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        setlayout(pMainLayout);

        m_pleFirstInfo = new QLineEdit(this);
        pMainLayout->addWidget(m_pleFirstInfo);

        m_pleSecondInfo = new QLineEidt(this);
        pMainLayout->addWidget(m_pleSecondInfo);

        m_pbtnRequest = new QPushButton(tr("request"), this);
        pMainLayout->addWidget(m_pbtnRequest);
    }

    void CRequestWidget::initTimer()
    {
        m_pTimer = new QTimer(this);
        bool VARIABLE_IS_NOT_USED bIsTimerConOK = connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateProgressBar()));
        Q_ASSERT(bIsTimerConOK);
    }

    void CRequestWidget::updateProgressBar()
    {
        while (s_requestStatus == GuiCommon::eRequesting)
        {
            ++m_elapsedTimes;
            if ((m_elapsedTimes * m_interval) < m_timeoutSec)
            {
                m_ppbProgress->setValue(m_elapsedTimes * m_interval);
            }
            else
            {
                s_requestStatus = GuiCommon::eTimeout;
            }
        }
        m_pTimer->stop();
        emit requestEnd(s_requestStatus);
    }

    void CRequestWidget::beginLogin()
    {
        s_requestStatus = GuiCommon::eRequesting;
        m_elapsedTimes = 0;
        m_pTimer->setInterval(m_interval);
        m_pTimer->start();
        qDebug() << m_pTimer->interval();
    }
}
