/**
 * @file loginwindow.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 * @modified  Thu 2015-12-03 00:31:16 (+0800)
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
		m_pleUserNumber = new QLineEdit(this);
        m_pleUserNumber->setPlaceholderText(tr("enter your user number"));
		m_plePasswd = new QLineEdit(this);
        m_plePasswd->setEchoMode(QLineEdit::Password);

		m_pbtnRegister = new Common::CWordsButton(tr("register"), this);
		m_pbtnRetrievePasswd = new Common::CWordsButton(tr("retrieve password"), this);
        m_pbtnLogin = new QPushButton(tr("login"), this);

		pMainLayout->addWidget(plbIndividuation, 0, 0, 6, 10);

		pMainLayout->addWidget(m_plbPhoto, 6, 0, 2, 3);
        pMainLayout->addWidget(m_pleUserNumber, 6, 3, 1, 5);
        pMainLayout->addWidget(m_plePasswd, 7, 3, 1, 5);
        pMainLayout->addWidget(m_pbtnRegister, 6, 9, 1, 1);
        pMainLayout->addWidget(m_pbtnRetrievePasswd, 7, 9, 1, 1);
        pMainLayout->addWidget(m_pbtnLogin, 9, 3, 1, 6);

        bool bIsLoginConOK = connect(m_pbtnLogin, SIGNAL(clicked(bool)), this, SIGNAL(login()));
        Q_ASSERT(bIsLoginConOK);
    }
}
