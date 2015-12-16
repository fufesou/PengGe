/**
 * @file loginwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Tue 2015-12-15 19:21:39 (+0800)
 */

#ifndef _LOGINWIDGET_H
#define _LOGINWIDGET_H

#include    "requestwidget.h"

QT_FORWARD_DECLARE_CLASS(QPushButton)

namespace GuiClient
{
    class CLoginWidget : public CRequestWidget
    {
        Q_OBJECT

    public:
        explicit CLoginWidget(QWidget* vParent = 0);

    signals:
        void switch2Register(void);
        void switch2Retrieve(void);

    private:
        void initWidget(void);

    private:
        QPushButton* m_pbtnRegister;
        QPushButton* m_pbtnRetrievePasswd;
    };
}

#endif //LOGINWIDGET_H
