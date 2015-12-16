/**
 * @file verifyingwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-16
 * @modified  Wed 2015-12-16 23:11:16 (+0800)
 */

#ifndef _VERIFYINGWIDGET_H
#define _VERIFYINGWIDGET_H

#include    "requestingwidget.h"

namespace GuiClient
{
    class CVerifyingWidget : public CRequestingWidget
    {
        Q_OBJECT

    public:
        explicit CVerifyingWidget(QWidget* vParent = 0, int vTimeoutMsec = 30*1000, int vIntervalMsec = 300);
    };
}

#endif //VERIFYINGWIDGET_H
