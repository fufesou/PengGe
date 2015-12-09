/**
 * @file logingwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Wed 2015-12-09 23:28:29 (+0800)
 */

#include  <QTime>
#include  <QLabel>
#include  <QProgressBar>
#include  <QPushButton>
#include  <QVBoxLayout>

#include    "guimacros.h"
#include    "logingwidget.h"

namespace GuiClient
{
    GuiCommon::ELoginStatus CLogingWidget::s_loginStatus = GuiCommon::eLoging;

    CLogingWidget::CLogingWidget(int vTimeoutSec, QWidget* vParent)
        : QWidget(vParent)
        , m_timeoutSec(vTimeoutSec)
    {
        initWidget();
    }

    void CLogingWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        setLayout(pMainLayout);

        m_plbScene = new QLabel(tr("set picture here"), this);
        pMainLayout->addWidget(this);

        m_ppbProgress = new QProgressBar(this);
        m_ppbProgress->setRange(0, m_timeoutSec);
        pMainLayout->addWidget(m_ppbProgress);

        m_pbtnCancel = new QPushButton(tr("cancel"), this);
        pMainLayout->addWidget(m_pbtnCancel);

        bool VARIABLE_IS_NOT_USED bIsCancelConOK = connect(m_pbtnCancel, SIGNAL(clicked()), this, SIGNAL(cancelLogin()));
        Q_ASSERT(bIsCancelConOK);
    }

    void CLogingWidget::beginLogin()
    {
        QTime timeBegin;
        timeBegin.start();
        m_ppbProgress->setValue(0);
        while (s_loginStatus == GuiCommon::eLoging)
        {
            int timeElapsed = timeBegin.elapsed();
            if (timeElapsed < m_timeoutSec)
            {
                m_ppbProgress->setValue(timeElapsed);
            }
            else
            {
                s_loginStatus = GuiCommon::eTimeout;
            }
        }
        emit loginEnd(s_loginStatus);
    }
}
