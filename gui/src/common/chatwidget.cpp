/**
 * @file chatwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-05
 * @modified  Sat 2015-12-05 21:59:57 (+0800)
 */

#include  <QLabel>
#include  <QListView>
#include  <QGridLayout>
#include  <QHBoxLayout>
#include  <QVBoxLayout>
#include  <QPushButton>
#include  <QTextEdit>
#include  <QScrollArea>
#include  <QScrollBar>
#include  <QTime>

#include    "guimacros.h"
#include    "chatwidget.h"

namespace GuiCommon
{
    CChatWidget::CChatWidget(const QSharedPointer<CDisplayItemInfo>& vItemInfo, QWidget* vParent)
        : QWidget(vParent)
        , m_pItemInfo(vItemInfo)
    {
        initWidget();

        setGeometry(400, 300, 600, 400);
    }

    CChatWidget::~CChatWidget()
    {
    }

    void CChatWidget::initWidget()
    {
        QGridLayout* pMainLayout = new QGridLayout(this);
        setLayout(pMainLayout);

        m_pChatListWidget = new QWidget(this);
        pMainLayout->addWidget(m_pChatListWidget, 0, 0, 10, 2, Qt::AlignLeft);

        m_pPeerInfoWidget = new QWidget(this);
        pMainLayout->addWidget(m_pPeerInfoWidget, 0, 2, 2, 8);
        QHBoxLayout* pPeerInfoLayout = new QHBoxLayout(m_pPeerInfoWidget);
        m_pPeerInfoWidget->setLayout(pPeerInfoLayout);
        pPeerInfoLayout->addWidget(new QLabel(tr("name"), m_pPeerInfoWidget));
        pPeerInfoLayout->addWidget(new QLabel(tr("ip"), m_pPeerInfoWidget));
        pPeerInfoLayout->addWidget(new QLabel(tr("port"), m_pPeerInfoWidget));

        m_pChatLogWidget = new QWidget(this);
        m_pChatLogWidget->setLayout(new QVBoxLayout(m_pChatLogWidget));
        QScrollArea* pLogArea = new QScrollArea(this);
        pLogArea->setWidget(m_pChatLogWidget);
        pLogArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pLogArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        pMainLayout->addWidget(pLogArea, 2, 2, 5, 8);

        pLogArea->setWidgetResizable(true);
        pLogArea->setStyleSheet(
                    "QWidget{background-color: transparent;} QScrollArea{background-color: rgba(255, 255, 255, 59);}");

        m_psbLog = pLogArea->verticalScrollBar();

        QWidget* pInputWidget = new QWidget(this);
        pMainLayout->addWidget(pInputWidget, 7, 2, 3, 8);
        QHBoxLayout* pInputLayout = new QHBoxLayout(pInputWidget);
        pInputWidget->setLayout(pInputLayout);
        m_pteInput = new QTextEdit(pInputWidget);
        pInputLayout->addWidget(m_pteInput);
        QPushButton* pbtnSend = new QPushButton(tr("send"), pInputWidget);
        pInputLayout->addWidget(pbtnSend);

        bool VARIABLE_IS_NOT_USED bIsSendConOK = connect(pbtnSend, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));
        Q_ASSERT(bIsSendConOK);
    }

    void CChatWidget::sendButtonClicked()
    {
        QString msg = m_pteInput->toPlainText();
        if (msg.size() > 0)
        {
            m_pteInput->clear();
            addMsg(msg, QString("this"), eSend);
            sendMessage(msg);
        }
    }

    void CChatWidget::addMsg(const QString& vMsg, const QString& vSenderName, EMsgSendRecv vSendRecv)
    {
        QLabel* pMsgItem = new QLabel(m_pChatLogWidget);
        QGridLayout* pMsgLayout = new QGridLayout(pMsgItem);
        pMsgItem->setLayout(pMsgLayout);
        QString strTime = QTime::currentTime().toString("HH:mm:ss");

        Qt::Alignment align = (vSendRecv == eSend) ? Qt::AlignRight : Qt::AlignLeft;
        pMsgLayout->addWidget(new QLabel(strTime, pMsgItem), 0, 0, 1, 10, align);
        pMsgLayout->addWidget(new QLabel(vMsg, pMsgItem), 1, 0, 1, 8, align);
        pMsgLayout->addWidget(new QLabel(vSenderName, pMsgItem), 1, 9, 1, 1, align);
        pMsgItem->setFixedHeight(60);

        m_pChatLogWidget->setFixedHeight(m_pChatLogWidget->height() + pMsgItem->height());
        m_pChatLogWidget->layout()->addWidget(pMsgItem);

        m_psbLog->setMaximumHeight(this->height());
        m_psbLog->setSliderPosition(m_psbLog->maximum());
    }
}
