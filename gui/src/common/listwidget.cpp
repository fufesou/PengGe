/**
 * @file listwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Fri 2016-01-01 11:25:35 (+0800)
 */

#include  <QDebug>
#include  <QFile>
#include  <QTextStream>
#include  <QStandardItemModel>

#include    "guimacros.h"
#include    "listwidget.h"
#include    "chatwidget.h"
#include    "displayiteminfo.h"
#include    "displayitemwidget.h"

namespace GuiCommon
{
    CListWidget::CListWidget(const QString& vAccountFile, QWidget* vParent)
        : QListView(vParent)
        , m_accountFile(vAccountFile)
    {
        initWidget();
        updateFriendList();
    }

    CListWidget::~CListWidget()
    {
        clearChatListWidget();
        updateAccountFile();
    }

    void CListWidget::initWidget()
    {
        m_pItemModel = new QStandardItemModel(this);
        setModel(m_pItemModel);
    }

    bool CListWidget::updateFriendList()
    {
        clearFriendList();

        QFile inFile(m_accountFile);
        if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "cannot read file " << m_accountFile;
            return false;
        }
        QTextStream inStream(&inFile);

        while (!inStream.atEnd())
        {
            QSharedPointer<CDisplayItemInfo> pNewItemInfo(new CDisplayItemInfo(inStream));
            if (!pNewItemInfo->fetchName().isEmpty())
            {
                m_friendList.push_back(pNewItemInfo);
                addItem(pNewItemInfo);
            }
        }

        return true;
    }

    bool CListWidget::updateAccountFile() const
    {
        QFile outFile(m_accountFile);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qDebug() << "cannot write file " << m_accountFile;
            return false;
        }
        QTextStream outStream(&outFile);

        foreach (QSharedPointer<CDisplayItemInfo> item, m_friendList)
        {
            item->writeStream(outStream);
        }

        return true;
    }

    void CListWidget::clearFriendList()
    {
        initWidget();
    }

    void CListWidget::clearChatListWidget()
    {
        foreach (QWidget* pWidget, m_listChatWidget)
        {
            delete pWidget;
        }
    }

    void CListWidget::addItem(QSharedPointer<CDisplayItemInfo> vNewItem)
    {
        QStandardItem *pItem = new QStandardItem();
        pItem->setSizeHint(QSize(50, 100));
        m_pItemModel->appendRow(pItem);
        QModelIndex index = m_pItemModel->indexFromItem(pItem);
        CDisplayItemWidget* pNewItemWidget = new CDisplayItemWidget(vNewItem, this);
        setIndexWidget(index, pNewItemWidget);

        bool VARIABLE_IS_NOT_USED bIsShowChatConOK = connect(
                    pNewItemWidget,
                    SIGNAL(doubleClicked(const QSharedPointer<CDisplayItemInfo>&)),
                    this,
                    SLOT(showChatWidget(const QSharedPointer<CDisplayItemInfo>&)));
        Q_ASSERT(bIsShowChatConOK);
    }

    void CListWidget::showChatWidget(const QSharedPointer<CDisplayItemInfo>& vItemInfo)
    {
        foreach (QWidget* pWidget, m_listChatWidget)
        {
            if (pWidget->windowTitle() == vItemInfo->getName())
            {
                pWidget->setGeometry(200, 200, 400, 500);
                pWidget->show();
                return;
            }
        }

        CChatWidget* pNewChatWidget(new CChatWidget(vItemInfo));
        pNewChatWidget->setWindowTitle(vItemInfo->getName());
        pNewChatWidget->setGeometry(200, 200, 400, 500);
        pNewChatWidget->show();
        m_listChatWidget.push_back(pNewChatWidget);
    }
}
