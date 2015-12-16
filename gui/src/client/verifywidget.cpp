/**
 * @file verifywidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-16
 * @modified  Wed 2015-12-16 23:04:36 (+0800)
 */

#include  <QLabel>
#include  <QPushButton>
#include  <QHBoxLayout>
#include  <QVBoxLayout>

#include    "guimacros.h"
#include    "verifywidget.h"

namespace GuiClient
{
    CVerifyWidget::CVerifyWidget(QWidget* vParent)
        : CRequestWidget(vParent)
    {
        initWidget();
    }

    void CVerifyWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = dynamic_cast<QVBoxLayout*>(layout());
        Q_ASSERT(pMainLayout);

        m_plbFirstInfo->setText(tr("telephone number:"));
        m_plbSecondInfo->setText(tr("random code:"));
        m_plbFirstInfo->setFixedWidth(180);
        m_plbSecondInfo->setFixedWidth(180);

        m_pbtnRequest->setText(tr("verify"));
        m_pbtnRequest->setMinimumWidth(100);
        m_pbtnRequest->setMaximumWidth(200);

        QHBoxLayout* pBtnsLayout = new QHBoxLayout;
        m_pbtnLogin = new QPushButton(tr("login window"), this);
        m_pbtnLogin->setMinimumWidth(100);
        m_pbtnLogin->setMaximumWidth(200);
        pBtnsLayout->addWidget(m_pbtnLogin);

        pMainLayout->addLayout(pBtnsLayout);

        bool VARIABLE_IS_NOT_USED bIsLoginConOK = connect(m_pbtnLogin, SIGNAL(clicked(bool)), this, SIGNAL(switch2Login()));
        Q_ASSERT(bIsLoginConOK);
    }
}
