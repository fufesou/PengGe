/**
 * @file requestingwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-11
 * @modified  Tue 2015-12-15 19:40:26 (+0800)
 */

#include  <QDebug>
#include  <QTimer>
#include  <QLabel>
#include  <QProgressBar>
#include  <QPushButton>
#include  <QVBoxLayout>

#include    "guimacros.h"
#include    "requestingwidget.h"

namespace GuiClient
{
    GuiCommon::ERequestStatus CRequestingWidget::s_requestStatus = GuiCommon::eRequesting;

    CRequestingWidget::CRequestingWidget(QWidget* vParent, int vTimeoutMsec, int vIntervalMsec)
        : QWidget(vParent)
        , m_timeoutMsec(vTimeoutMsec)
        , m_interval(vIntervalMsec)
        , m_pTimer(NULL)
    {
        initWidget();
        initTimer();

        setGeometry(200, 200, 500, 400);
        setFixedSize(500, 400);
    }

    void CRequestingWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        setLayout(pMainLayout);

        m_plbScene = new QLabel(tr("set picture here"), this);
        pMainLayout->addWidget(m_plbScene);

        m_ppbProgress = new QProgressBar(this);
        m_ppbProgress->setRange(0, m_timeoutMsec);
        pMainLayout->addWidget(m_ppbProgress);
    }

    void CRequestingWidget::initTimer()
    {
        m_pTimer = new QTimer(this);
        bool VARIABLE_IS_NOT_USED bIsTimerConOK = connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateProgressBar()));
        Q_ASSERT(bIsTimerConOK);
    }

    void CRequestingWidget::updateProgressBar()
    {
        while (s_requestStatus == GuiCommon::eRequesting)
        {
            ++m_elapsedTimes;
            qDebug() << m_elapsedTimes;
            // if ((m_elapsedTimes * m_interval) < m_timeoutMsec)
            if ((m_elapsedTimes * 300) < 30*1000)
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

    void CRequestingWidget::beginRequest()
    {
        s_requestStatus = GuiCommon::eRequesting;
        m_elapsedTimes = 0;
        // m_pTimer->setInterval(m_interval);
        m_pTimer->setInterval(300);
        m_pTimer->start();
        qDebug() << m_pTimer->interval();
    }

}
