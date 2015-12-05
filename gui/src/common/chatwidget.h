/**
 * @file chatwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-05
 * @modified  Sat 2015-12-05 21:59:10 (+0800)
 */

#ifndef _CHATWIDGET_H
#define _CHATWIDGET_H

#include  <QWidget>

#include    "guitypes.h"

QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QScrollBar)

namespace GuiCommon
{
    class CChatWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CChatWidget(QWidget* vParent = 0);
        virtual ~CChatWidget();

        void addMsg(const QString& vMsg, const QString& vSenderName, EMsgSendRecv vSendRecv);

    private:
        void initWidget(void);

    signals:
        void sendMessage(const QString& vMsg);

    private slots:
        void sendButtonClicked();

    private:
        QWidget* m_pListWidget;
        QWidget* m_pPeerInfoWidget;
        QWidget* m_pLogWidget;
        QScrollBar* m_psbLog;
        QTextEdit* m_pteInput;
    };
}

#endif //CHATWIDGET_H
