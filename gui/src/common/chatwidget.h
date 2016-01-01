/**
 * @file chatwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-05
 * @modified  Fri 2016-01-01 11:08:33 (+0800)
 */

#ifndef _CHATWIDGET_H
#define _CHATWIDGET_H

#include  <QWidget>
#include  <QSharedPointer>

#include    "guitypes.h"

QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QScrollBar)

namespace GuiCommon
{
    QT_FORWARD_DECLARE_CLASS(CDisplayItemInfo)

    class CChatWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CChatWidget(const QSharedPointer<CDisplayItemInfo>& vItemInfo, QWidget* vParent = 0);
        virtual ~CChatWidget();

        void addMsg(const QString& vMsg, const QString& vSenderName, EMsgSendRecv vSendRecv);

    private:
        void initWidget(void);

    signals:
        void sendMessage(const QString& vMsg);

    private slots:
        void sendButtonClicked();

    private:
        QWidget* m_pChatListWidget;
        QWidget* m_pPeerInfoWidget;
        QWidget* m_pRecordWidget;
        QScrollBar* m_psbRecord;
        QTextEdit* m_pteInput;
        QSharedPointer<CDisplayItemInfo> m_pItemInfo;
    };
}

#endif //CHATWIDGET_H
