/**
 * @file mainwidget.cpp
 * @brief  
 * @author cxl, hermes-sys, <xiaolong.chen@hermes-sys.com>
 * @version 0.1
 * @date 2015-12-03
 * @modified  ���� 2015-12-03 21:05:01 �й���׼ʱ��
 */

#include  <QGridLayout>

#include    "mainwidget.h"

namespace GuiClient
{
    CMainWidget::CMainWidget(QWidget* vParent)
        : QWidget(vParent)
    {
        this->setGeometry(600, 100, 200, 600);
    }

    void CMainWidget::initWidgets()
    {
        QGridLayout* pMainLayout = new QGridLayout(this);
        this->setLayout(pMainLayout);

        m_pSelfInfoWidget = new QWidget(this);
        pMainLayout->addWidget(m_pSelfInfoWidget, 0, 0, 2, 1);

        m_pMainDisplayWidget = new QWidget(this);
        pMainLayout->addWidget(m_pMainDisplayWidget, 2, 0, 6, 1);

        m_pCtrlItemsWidget = new QWidget(this);
        pMainLayout->addWidget(m_pCtrlItemsWidget, 8, 0, 2, 1);
    }
}
