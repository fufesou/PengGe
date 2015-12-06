/**
 * @file displayitemwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 */

#include  <QLabel>
#include  <QPushButton>
#include  <QGridLayout>

#include    "displayitemwidget.h"
#include    "displayiteminfo.h"

namespace GuiCommon
{
    CDisplayItemWidget::CDisplayItemWidget(const QSharedPointer<CDisplayItemInfo>& vItemInfo, QWidget* vParent)
        : QWidget(vParent)
        , m_pItemInfo(vItemInfo)
    {
        initWidget();
    }

    void CDisplayItemWidget::initWidget()
    {
        QGridLayout* pMainLayout = new QGridLayout(this);
        setLayout(pMainLayout);

        m_plbPortrait = new QLabel(this);
        m_plbPortrait->setPixmap(QPixmap::fromImage(QImage(m_pItemInfo->fetchPortraitPath())));
        m_plbPortrait->setScaledContents(true);
        pMainLayout->addWidget(m_plbPortrait, 0, 0, 2, 2);

        m_plbPeerName = new QLabel(m_pItemInfo->fetchName(), this);
        pMainLayout->addWidget(m_plbPeerName, 0, 2, 1, 2);

        m_plbOtherInfo = new QLabel(m_pItemInfo->fetchSimpleInfo(), this);
        pMainLayout->addWidget(m_plbOtherInfo, 1, 2, 1, 4);

        m_pbtnInfo = new QPushButton(tr("peer detail info"), this);
        pMainLayout->addWidget(m_pbtnInfo, 0, 4, 1, 1);
    }
}
