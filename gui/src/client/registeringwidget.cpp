/**
 * @file registeringwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Tue 2015-12-15 19:41:32 (+0800)
 */

#include    "registeringwidget.h"

namespace GuiClient
{
    CRegisterWidget::CRegisterWidget(QWidget* vParent, int vTimeoutMsec, int vIntervalMsec)
        : CRequestingWidget(vTimeoutMsec, vIntervalMsec, vParent)
    {
    }
}
