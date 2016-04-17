/**
 * @file registeringwidget.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Wed 2015-12-16 23:10:49 (+0800)
 */

#ifndef _REGISTERINGWIDGET_H
#define _REGISTERINGWIDGET_H

#include    "requestingwidget.h"

namespace GuiClient
{
    class CRegisteringWidget : public CRequestingWidget
    {
        Q_OBJECT

    public:
        explicit CRegisteringWidget(QWidget* vParent = 0, int vTimeoutMsec = 30*1000, int vIntervalMsec = 300);
    };
}

#endif //REGISTERINGWIDGET_H
