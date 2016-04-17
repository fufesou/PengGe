/**
 * @file loginingwidget.cpp
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Tue 2015-12-15 19:41:09 (+0800)
 */

#include    "loginingwidget.h"

namespace GuiClient
{
    CLoginingWidget::CLoginingWidget(QWidget* vParent, int vTimeoutMsec, int vIntervalMsec)
        : CRequestingWidget(vParent, vTimeoutMsec, vIntervalMsec)
    {
    }
}
