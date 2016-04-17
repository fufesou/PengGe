/**
 * @file registerwidget.cpp
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Wed 2015-12-16 23:08:43 (+0800)
 */

#include  <QLabel>
#include  <QPushButton>
#include  <QHBoxLayout>
#include  <QVBoxLayout>

#include    "guimacros.h"
#include    "registerwidget.h"

namespace GuiClient
{
    CRegisterWidget::CRegisterWidget(QWidget* vParent)
        : CRequestWidget(vParent)
    {
        initWidget();
    }

    void CRegisterWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = dynamic_cast<QVBoxLayout*>(layout());
        Q_ASSERT(pMainLayout);

        m_plbFirstInfo->setText(tr("user number:"));
        m_plbSecondInfo->setText(tr("telephone number:"));
        m_plbFirstInfo->setFixedWidth(180);
        m_plbSecondInfo->setFixedWidth(180);

        m_pbtnRequest->setText(tr("register"));
        m_pbtnRequest->setMinimumWidth(100);
        m_pbtnRequest->setMaximumWidth(200);

        QHBoxLayout* pBtnsLayout = new QHBoxLayout;
        m_pbtnLogin = new QPushButton(tr("login window"), this);
        pBtnsLayout->addWidget(m_pbtnLogin);
        m_pbtnVerify = new QPushButton(tr("verify window"), this);
        pBtnsLayout->addWidget(m_pbtnVerify);

        pMainLayout->addLayout(pBtnsLayout);

        bool VARIABLE_IS_NOT_USED bIsLoginConOK = connect(m_pbtnLogin, SIGNAL(clicked(bool)), this, SIGNAL(switch2Login()));
        Q_ASSERT(bIsLoginConOK);

        bool VARIABLE_IS_NOT_USED bIsVerifyConOK = connect(m_pbtnVerify, SIGNAL(clicked(bool)), this, SIGNAL(switch2Verify()));
        Q_ASSERT(bIsVerifyConOK);
    }
}
