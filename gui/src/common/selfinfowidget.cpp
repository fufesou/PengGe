/**
 * @file selfinfowidget.cpp
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Tue 2015-12-08 00:43:27 (+0800)
 */

#include  <QVBoxLayout>
#include  <QHBoxLayout>
#include  <QGroupBox>
#include  <QLabel>

#include    "selfinfowidget.h"

namespace GuiCommon
{
    CSelfInfoWidget::CSelfInfoWidget(QWidget* vParent)
        : QWidget(vParent)
    {
        initWidget();
    }

    CSelfInfoWidget::~CSelfInfoWidget()
    {
    }

    void CSelfInfoWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        setLayout(pMainLayout);

        QGroupBox* pgrpSocketInfo = new QGroupBox(tr("socket info"), this);
        pMainLayout->addWidget(pgrpSocketInfo);
        QHBoxLayout* pSockLayout = new QHBoxLayout(pgrpSocketInfo);
        pgrpSocketInfo->setLayout(pSockLayout);
        pSockLayout->addWidget(new QLabel(tr("ip")));
        pSockLayout->addWidget(new QLabel(tr("port")));

        QGroupBox* pgrpUserInfo = new QGroupBox(tr("user info"), this);
        pMainLayout->addWidget(pgrpUserInfo);
        QHBoxLayout* pUserLayout = new QHBoxLayout(pgrpUserInfo);
        pgrpUserInfo->setLayout(pUserLayout);
        pUserLayout->addWidget(new QLabel(tr("user name")));
        pUserLayout->addWidget(new QLabel(tr("user grade")));
    }
}
