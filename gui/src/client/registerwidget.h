/**
 * @file registerwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Wed 2015-12-16 23:06:45 (+0800)
 */

#ifndef _REGISTERWIDGET_H
#define _REGISTERWIDGET_H

#include    "requestwidget.h"

namespace GuiClient
{
    class CRegisterWidget : public CRequestWidget
    {
        Q_OBJECT

    public:
        explicit CRegisterWidget(QWidget* vParent = 0);

    signals:
        void switch2Login(void);
        void switch2Verify(void);

    private:
        void initWidget(void);

    private:
        QPushButton* m_pbtnLogin;
        QPushButton* m_pbtnVerify;
    };
}

#endif //REGISTERWIDGET_H
