/**
 * @file listwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 */

#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include  <QSharedPointer>
#include  <QListView>

QT_FORWARD_DECLARE_CLASS(QStandardItemModel)

namespace GuiCommon
{
    QT_FORWARD_DECLARE_CLASS(CDisplayItemInfo)

    class CListWidget : public QListView
    {
        Q_OBJECT

    public:
        explicit CListWidget(QWidget* vParent = 0);

    private:
        void initWidget(void);
        void addItem(QSharedPointer<CDisplayItemInfo> vNewItem);

    private:
        QStandardItemModel* m_pItemModel;
    };
}

#endif // LISTWIDGET_H
