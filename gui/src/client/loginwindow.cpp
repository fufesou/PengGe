/**
 * @file loginwindow.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 * @modified  Thu 2015-12-10 18:54:28 (+0800)
 */

#include  <QPushButton>
#include  <QLineEdit>
#include  <QLabel>
#include  <QGridLayout>

#include    "loginwindow.h"
#include    "../common/wordsbutton.h"

namespace GuiClient
{
	CLoginWindow::CLoginWindow(QWidget* vParent)
		: QWidget(vParent)
	{
		initWidgets();

        setGeometry(200, 200, 500, 400);
        setFixedSize(500, 400);
	}

	void CLoginWindow::initWidgets()
	{
		QGridLayout* pMainLayout = new QGridLayout(this);
		this->setLayout(pMainLayout);

        QLabel* plbIndividuation = new QLabel(this);
        plbIndividuation->setStyleSheet(
                    "QLabel {"
                        "image: url(:/img/aa0.jpg);"
                    "}");

		m_plbPhoto = new QLabel(tr("photo here"), this);
		m_pleUserInfo = new QLineEdit(this);
        m_pleUserInfo->setMinimumWidth(250);
        m_pleUserInfo->setPlaceholderText(tr("enter your user number"));
		m_plePasswd = new QLineEdit(this);
        m_plePasswd->setMinimumWidth(250);
        m_plePasswd->setEchoMode(QLineEdit::Password);

        m_pbtnLogin = new QPushButton(tr("login"), this);
        m_pbtnLogin->setMinimumWidth(100);
        m_pbtnRegister = new QPushButton(tr("register"), this);
        m_pbtnRegister->setMinimumWidth(100);
        m_pbtnRetrievePasswd = new QPushButton(tr("retrieve password"), this);
        m_pbtnRetrievePasswd->setMinimumWidth(100);


		pMainLayout->addWidget(plbIndividuation, 0, 0, 6, 10);

        pMainLayout->addWidget(m_plbPhoto, 6, 0, 2, 2, Qt::AlignLeft);
        pMainLayout->addWidget(m_pleUserInfo, 6, 2, 1, 5, Qt::AlignLeft);
        pMainLayout->addWidget(m_plePasswd, 7, 2, 1, 5, Qt::AlignLeft);
        pMainLayout->addWidget(m_pbtnLogin, 6, 7, 1, 2, Qt::AlignLeft);
        pMainLayout->addWidget(m_pbtnRegister, 8, 2, 1, 2, Qt::AlignLeft);
        pMainLayout->addWidget(m_pbtnRetrievePasswd, 8, 6, 1, 2, Qt::AlignLeft);

        bool bIsLoginConOK = connect(m_pbtnLogin, SIGNAL(clicked(bool)), this, SLOT(beginLogin()));
        Q_ASSERT(bIsLoginConOK);
    }

    void CLoginWindow::beginLogin()
    {
        emit login(m_pleUserInfo->text(), m_plePasswd->text());
    }
}
