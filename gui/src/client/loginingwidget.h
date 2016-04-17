/**
 * @file loginingwidget.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Tue 2015-12-15 19:40:47 (+0800)
 */

#ifndef _LOGININGWIDGET_H
#define _LOGININGWIDGET_H

#include    "requestingwidget.h"

namespace GuiClient
{
    class CLoginingWidget : public CRequestingWidget
    {
        Q_OBJECT

    public:
        explicit CLoginingWidget(QWidget* vParent = 0, int vTimeoutMsec = 30*1000, int vIntervalMsec = 300);
    };
}

#endif //LOGININGWIDGET_H
