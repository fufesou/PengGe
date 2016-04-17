/**
 * @file listwidget.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Sun 2015-12-20 13:17:56 (+0800)
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
        explicit CListWidget(const QString& vAccountFile, QWidget* vParent = 0);
        ~CListWidget();

        void addItem(QSharedPointer<CDisplayItemInfo> vNewItem);
        bool updateFriendList(void);

        bool updateAccountFile(void) const;

        void clearFriendList(void);

    private:
        void initWidget(void);
        void clearChatListWidget(void);

    private slots:
        void showChatWidget(const QSharedPointer<CDisplayItemInfo>& vItemInfo);

    private:
        QString 			m_accountFile;
        QList<QWidget*> 	m_listChatWidget;
        QStandardItemModel* m_pItemModel;
        QList<QSharedPointer<CDisplayItemInfo> > m_friendList;
    };
}

#endif // LISTWIDGET_H
