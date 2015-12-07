/**
 * @file logingwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Tue 2015-12-08 00:33:27 (+0800)
 */

#include  <QPushButton>
#include  <QVBoxLayout>

#include    "guimacros.h"
#include    "logingwidget.h"

namespace GuiClient
{
    CLogingWidget::CLogingWidget(QWidget* vParent)
        : QWidget(vParent)
    {
        initWidget();
    }

    void CLogingWidget::initWidget()
    {
        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        setLayout(pMainLayout);

        m_pbtnCancel = new QPushButton(tr("cancel"), this);
        pMainLayout->addWidget(m_pbtnCancel);

        bool VARIABLE_IS_NOT_USED bIsCancelConOK = connect(m_pbtnCancel, SIGNAL(clicked()), this, SIGNAL(cancelLogin()));
        Q_ASSERT(bIsCancelConOK);
    }
}
