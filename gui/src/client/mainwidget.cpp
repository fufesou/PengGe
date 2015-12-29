/**
 * @file mainwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Sun 2015-12-20 16:37:15 (+0800)
 */

#include  <QPushButton>
#include  <QGridLayout>

#include    "guimacros.h"
#include    "mainwidget.h"
#include    "displayiteminfo.h"
#include    "listwidget.h"
#include    "selfinfowidget.h"

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

        m_pSelfInfoWidget = new GuiCommon::CSelfInfoWidget(this);
        pMainLayout->addWidget(m_pSelfInfoWidget, 0, 0, 2, 1);

        m_pMainDisplayWidget = new GuiCommon::CListWidget(QString("../cli_account_data.txt"), this);
        pMainLayout->addWidget(m_pMainDisplayWidget, 2, 0, 6, 1);

        m_pCtrlItemsWidget = new QWidget(this);
        pMainLayout->addWidget(m_pCtrlItemsWidget, 8, 0, 2, 1);
        QGridLayout* pCtrlWidgetLayout = new QGridLayout();
        m_pCtrlItemsWidget->setLayout(pCtrlWidgetLayout);
        QPushButton* pbtnLogout = new QPushButton(tr("logout"), m_pCtrlItemsWidget);
        pCtrlWidgetLayout->addWidget(pbtnLogout, 0, 0, 1, 1, Qt::AlignLeft);

        bool VARIABLE_IS_NOT_USED bIsLogoutConOK = connect(pbtnLogout, SIGNAL(clicked(bool)), this, SIGNAL(logout()));
        Q_ASSERT(bIsLogoutConOK);
    }
}
