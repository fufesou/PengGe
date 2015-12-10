/**
 * @file loginwindow.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-02
 * @modified  Thu 2015-12-10 18:54:34 (+0800)
 */

#ifndef _LOGINWINDOW_H
#define _LOGINWINDOW_H

#include  <QWidget>

QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QLabel)

namespace GuiClient
{
    class CLoginWindow : public QWidget
	{
		Q_OBJECT

	public:
        explicit CLoginWindow(QWidget* vParent = 0);

	private:
		void initWidgets(void);

    signals:
        void login(const QString& vUserInfo, const QString& vPasswd);

    private slots:
        void beginLogin(void);

	private:
		QPushButton* m_pbtnRegister;
		QPushButton* m_pbtnRetrievePasswd;
		QPushButton* m_pbtnLogin;

		QLineEdit* m_pleUserInfo;
		QLineEdit* m_plePasswd;

		QLabel* m_plbPhoto;
    };
}

#endif //LOGINWINDOW_H
