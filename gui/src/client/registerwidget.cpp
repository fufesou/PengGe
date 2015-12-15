/**
 * @file registerwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Tue 2015-12-15 19:23:33 (+0800)
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
        m_pbtnVerify = new QPushButton(tr("verify account"), this);
        pBtnsLayout->addWidget(m_pbtnVerify);

        pMainLayout->addLayout(pBtnsLayout);

        bool VARIABLE_IS_NOT_USED bIsRegisterConOK = connect(m_pbtnRequest, SIGNAL(clicked(bool)), this, SIGNAL(registerAccount()));
        Q_ASSERT(bIsRegisterConOK);

        bool VARIABLE_IS_NOT_USED bIsVerifyConOK = connect(m_pbtnVerify, SIGNAL(clicked(bool)), this, SIGNAL(verifyAccount()));
        Q_ASSERT(bIsVerifyConOK);
    }
}
