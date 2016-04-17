/**
 * @file verifywidget.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-16
 * @modified  Wed 2015-12-16 22:54:12 (+0800)
 */

#ifndef _VERIFYWIDGET_H
#define _VERIFYWIDGET_H

#include    "requestwidget.h"

namespace GuiClient
{
    class CVerifyWidget : public CRequestWidget
    {
        Q_OBJECT

    public:
        explicit CVerifyWidget(QWidget* vParent = 0);

    signals:
        void switch2Login(void);

    private:
        void initWidget(void);

    private:
        QPushButton* m_pbtnLogin;
    };
}


#endif //VERIFYWIDGET_H
