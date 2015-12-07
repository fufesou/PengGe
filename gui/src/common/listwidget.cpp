/**
 * @file listwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Tue 2015-12-08 00:42:42 (+0800)
 */

#include  <QStandardItemModel>

#include    "listwidget.h"
#include    "displayiteminfo.h"
#include    "displayitemwidget.h"

namespace GuiCommon
{
    CListWidget::CListWidget(QWidget* vParent)
        : QListView(vParent)
    {
        initWidget();
    }

    void CListWidget::initWidget()
    {
        m_pItemModel = new QStandardItemModel(this);
        setModel(m_pItemModel);

        addItem(QSharedPointer<CDisplayItemInfo>(
                    new CDisplayItemInfo(tr("peer name"), tr("D:/project/Qt/Test/TestListView/strawberry_128px.png"), tr("simple info"))));
        addItem(QSharedPointer<CDisplayItemInfo>(
            new CDisplayItemInfo(tr("peer name 2"), tr("D:/project/Qt/Test/TestListView/strawberry_128px.png"), tr("simple info2"))));
    }

    void CListWidget::addItem(QSharedPointer<CDisplayItemInfo> vNewItem)
    {
        QStandardItem *pItem = new QStandardItem();
        pItem->setSizeHint(QSize(50, 100));
        m_pItemModel->appendRow(pItem);
        QModelIndex index = m_pItemModel->indexFromItem(pItem);
        CDisplayItemWidget* pNewItemWidget = new CDisplayItemWidget(vNewItem, this);
        setIndexWidget(index, pNewItemWidget);
    }
}
