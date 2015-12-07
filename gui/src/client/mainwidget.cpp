/**
 * @file mainwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Tue 2015-12-08 00:35:09 (+0800)
 */

#include  <QGridLayout>

#include    "guimacros.h"
#include    "mainwidget.h"
#include    "displayiteminfo.h"

namespace GuiClient
{
    CMainWidget::CMainWidget(QWidget* vParent)
        : QWidget(vParent)
    {
        initWidget();
        setGeometry(600, 100, 200, 600);
    }

    void CMainWidget::initWidget()
    {
        QGridLayout* pMainLayout = new QGridLayout(this);
        setLayout(pMainLayout);

        m_pSelfInfoWidget = new QWidget(this);
        pMainLayout->addWidget(m_pSelfInfoWidget, 0, 0, 2, 1);

        m_pMainDisplayWidget = new QWidget(this);
        pMainLayout->addWidget(m_pMainDisplayWidget, 2, 0, 6, 1);

        m_pCtrlItemsWidget = new QWidget(this);
        pMainLayout->addWidget(m_pCtrlItemsWidget, 8, 0, 2, 1);
    }
}
