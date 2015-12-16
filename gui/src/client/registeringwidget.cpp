/**
 * @file registeringwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-15
 * @modified  Wed 2015-12-16 23:10:55 (+0800)
 */

#include    "registeringwidget.h"

namespace GuiClient
{
    CRegisteringWidget::CRegisteringWidget(QWidget* vParent, int vTimeoutMsec, int vIntervalMsec)
        : CRequestingWidget(vParent, vTimeoutMsec, vIntervalMsec)
    {
    }
}
