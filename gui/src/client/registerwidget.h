/**
 * @file registerwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Tue 2015-12-15 19:21:54 (+0800)
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
        void registerAccount(void);
        void verifyAccount(void);

    private:
        void initWidget(void);

    private:
        QPushButton* m_pbtnVerify;
    };
}

#endif //REGISTERWIDGET_H
