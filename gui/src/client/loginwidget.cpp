/**
 * @file loginwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Tue 2015-12-15 19:21:29 (+0800)
 */

#include  <QLabel>
#include  <QPushButton>
#include  <QLineEdit>
#include  <QHBoxLayout>
#include  <QVBoxLayout>

#include    "guimacros.h"
#include    "loginwidget.h"

namespace GuiClient
{
    CLoginWidget::CLoginWidget(QWidget* vParent)
        : CRequestWidget(vParent)
    {
        initWidget();
    }

    void CLoginWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = dynamic_cast<QVBoxLayout*>(layout());
        Q_ASSERT(pMainLayout);

        m_plbFirstInfo->setText(tr("user information:"));
        m_plbSecondInfo->setText(tr("password:"));
        m_plbFirstInfo->setFixedWidth(180);
        m_plbSecondInfo->setFixedWidth(180);

        m_pleSecondInfo->setEchoMode(QLineEdit::Password);

        m_pbtnRequest->setText(tr("login"));
        m_pbtnRequest->setMinimumWidth(100);
        m_pbtnRequest->setMaximumWidth(200);

        QHBoxLayout* pBtnsLayout = new QHBoxLayout;
        m_pbtnRegister = new QPushButton(tr("register account"), this);
        pBtnsLayout->addWidget(m_pbtnRegister);
        m_pbtnRetrievePasswd = new QPushButton(tr("retrieve password"), this);
        pBtnsLayout->addWidget(m_pbtnRetrievePasswd);

        pMainLayout->addLayout(pBtnsLayout);

        bool VARIABLE_IS_NOT_USED bIsRegisterConOK = connect(m_pbtnRegister, SIGNAL(clicked(bool)), this, SIGNAL(switch2Register()));
        Q_ASSERT(bIsRegisterConOK);

        bool VARIABLE_IS_NOT_USED bIsRetrieveConOK = connect(m_pbtnRetrievePasswd, SIGNAL(clicked(bool)), this, SIGNAL(switch2Retrieve()));
        Q_ASSERT(bIsRetrieveConOK);
    }
}
