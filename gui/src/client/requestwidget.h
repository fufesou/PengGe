/**
 * @file requestwidget.h
 * @brief  CRequestWidget is the base widget for processing request(register, verify, login, etc).
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-10
 * @modified  Tue 2015-12-15 14:37:02 (+0800)
 */

#ifndef _REQUESTWIDGET_H
#define _REQUESTWIDGET_H

#include  <QWidget>

#include    "guitypes.h"

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QPushButton)

namespace GuiClient
{
    class CRequestWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CRequestWidget(QWidget* vParent = 0);
        virtual ~CRequestWidget() {}

    signals:
        void request(const QString& vFirstInfo, const QString& vSecondInfo);

    private:
        void initWidget(void);

    private slots:
        void beginRequest(void);

    protected:
        QLabel* m_plbFirstInfo;
        QLabel* m_plbSecondInfo;
        QLineEdit* m_pleFirstInfo;
        QLineEdit* m_pleSecondInfo;

        QPushButton* m_pbtnRequest;
    };
}

#endif //REQUESTWIDGET_H
