/**
 * @file requestwidget.cpp
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-11
 * @modified  Tue 2015-12-15 17:55:09 (+0800)
 */

#include  <QPushButton>
#include  <QLineEdit>
#include  <QLabel>
#include  <QHBoxLayout>
#include  <QVBoxLayout>

#include    "requestwidget.h"

namespace GuiClient
{
    CRequestWidget::CRequestWidget(QWidget* vParent)
        : QWidget(vParent)
        , m_pleFirstInfo(NULL)
        , m_pleSecondInfo(NULL)
    {
        initWidget();

        setGeometry(200, 200, 500, 400);
        setFixedSize(500, 400);
    }

    void CRequestWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        setLayout(pMainLayout);

        QLabel* plbIndividuation = new QLabel(this);
        plbIndividuation->setStyleSheet(
                    "QLabel {"
                        "image: url(:/img/aa0.jpg);"
                    "}");
		pMainLayout->addWidget(plbIndividuation);

        QHBoxLayout* pFirstInfoLayout = new QHBoxLayout;
        m_plbFirstInfo = new QLabel(tr("first info"), this);
        pFirstInfoLayout->addWidget(m_plbFirstInfo);

		m_pleFirstInfo = new QLineEdit(this);
        m_pleFirstInfo->setMinimumWidth(250);
        m_pleFirstInfo->setPlaceholderText(tr("enter first info"));
        pFirstInfoLayout->addWidget(m_pleFirstInfo);
        pMainLayout->addLayout(pFirstInfoLayout);

        QHBoxLayout* pSecondInfoLayout = new QHBoxLayout;
        m_plbSecondInfo = new QLabel(tr("second info"));
        pSecondInfoLayout->addWidget(m_plbSecondInfo);

		m_pleSecondInfo = new QLineEdit(this);
        m_pleSecondInfo->setMinimumWidth(250);
        pSecondInfoLayout->addWidget(m_pleSecondInfo);
        pMainLayout->addLayout(pSecondInfoLayout);

        QHBoxLayout* pBtnLayout = new QHBoxLayout;
        m_pbtnRequest = new QPushButton(tr("request"), this);
        m_pbtnRequest->setMinimumWidth(100);
        pBtnLayout->addWidget(m_pbtnRequest);
        pMainLayout->addLayout(pBtnLayout);

        bool bIsRequestConOK = connect(m_pbtnRequest, SIGNAL(clicked(bool)), this, SLOT(beginRequest()));
        Q_ASSERT(bIsRequestConOK);
    }

    void CRequestWidget::beginRequest()
    {
        emit request(m_pleFirstInfo->text(), m_pleSecondInfo->text());
    }
}
