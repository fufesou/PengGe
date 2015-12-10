/**
 * @file registerwindow.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-10
 * @modified  Thu 2015-12-10 23:39:10 (+0800)
 */

#ifndef _REGISTERWINDOW_H
#define _REGISTERWINDOW_H

#include  <QWidget>

namespace GuiClient
{
    class CRegisterWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CRegisterWidget(QWidget* vParent = 0);

    signals:
        void registerAccount(const QString& vUserNum, const QString& vTelNum);
        void showBack(void);

    private slots:
        void doRegister(const QString& vUserNum, const QString& vTelNum);

    private:
        QLabel* m_plbResult;
        QLabel* m_plbUserNum;
        QLabel* m_plbTelNum;
        QLineEdit* m_pleUserNum;
        QLineEdit* m_plbTelNum;
        QPushButton* m_pbtnRegister;
        QPushButton* m_pbtnReturn;
    };
}


#endif //REGISTERWINDOW_H
