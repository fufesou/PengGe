/**
 * @file logingwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Wed 2015-12-09 23:28:29 (+0800)
 */

#include  <QDebug>
#include  <QTimer>
#include  <QLabel>
#include  <QProgressBar>
#include  <QPushButton>
#include  <QVBoxLayout>

#include    "guimacros.h"
#include    "logingwidget.h"

namespace GuiClient
{
    GuiCommon::ELoginStatus CLogingWidget::s_loginStatus = GuiCommon::eLoging;

    CLogingWidget::CLogingWidget(int vTimeoutSec, int vIntervalMsec, QWidget* vParent)
        : QWidget(vParent)
        , m_timeoutSec(vTimeoutSec)
        , m_interval(vIntervalMsec)
        , m_pTimer(NULL)
    {
        initWidget();
        initTimer();

        setGeometry(200, 200, 500, 400);
        setFixedSize(500, 400);
    }

    void CLogingWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        setLayout(pMainLayout);

        m_plbScene = new QLabel(tr("set picture here"), this);
        pMainLayout->addWidget(m_plbScene);

        m_ppbProgress = new QProgressBar(this);
        m_ppbProgress->setRange(0, m_timeoutSec);
        pMainLayout->addWidget(m_ppbProgress);

        m_pbtnCancel = new QPushButton(tr("cancel"), this);
        pMainLayout->addWidget(m_pbtnCancel);

        bool VARIABLE_IS_NOT_USED bIsCancelConOK = connect(m_pbtnCancel, SIGNAL(clicked()), this, SIGNAL(cancelLogin()));
        Q_ASSERT(bIsCancelConOK);
    }

    void CLogingWidget::initTimer()
    {
        m_pTimer = new QTimer(this);
        bool VARIABLE_IS_NOT_USED bIsTimerConOK = connect(m_pTimer, SIGNAL(timeout()), this, SLOT(updateProgressBar()));
        Q_ASSERT(bIsTimerConOK);
    }

    void CLogingWidget::updateProgressBar()
    {
        while (s_loginStatus == GuiCommon::eLoging)
        {
            ++m_elapsedTimes;
            if ((m_elapsedTimes * m_interval) < m_timeoutSec)
            {
                m_ppbProgress->setValue(m_elapsedTimes * m_interval);
            }
            else
            {
                s_loginStatus = GuiCommon::eTimeout;
            }
        }
        m_pTimer->stop();
        emit loginEnd(s_loginStatus);
    }

    void CLogingWidget::beginLogin()
    {
        s_loginStatus = GuiCommon::eLoging;
        m_elapsedTimes = 0;
        m_pTimer->setInterval(m_interval);
        m_pTimer->start();
        qDebug() << m_pTimer->interval();
    }
}
