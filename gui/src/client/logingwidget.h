/**
 * @file logingwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Tue 2015-12-08 00:32:05 (+0800)
 */

#include  <QWidget>

#ifndef _LOGINGWIDGET_H
#define _LOGINGWIDGET_H

QT_FORWARD_DECLARE_CLASS(QPushButton)

namespace GuiClient
{
    class CLogingWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CLogingWidget(QWidget* vParent = 0);

    signals:
        void cancelLogin(void);

    private:
        void initWidget(void);

    private:
        QPushButton* m_pbtnCancel;
    };
}


#endif //LOGINGWIDGET_H
