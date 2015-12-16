/**
 * @file verifyingwidget.cpp
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-16
 * @modified  Wed 2015-12-16 23:11:20 (+0800)
 */

#include    "verifyingwidget.h"

namespace GuiClient
{
    CVerifyingWidget::CVerifyingWidget(QWidget* vParent, int vTimeoutMsec, int vIntervalMsec)
        : CRequestingWidget(vParent, vTimeoutMsec, vIntervalMsec)
    {
    }
}
