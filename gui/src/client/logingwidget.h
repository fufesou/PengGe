/**
 * @file logingwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  周四 2015-12-03 23:04:48 中国标准时间
 */

#include  <QWidget>

#ifndef _LOGINGWIDGET_H
#define _LOGINGWIDGET_H

namespace GuiClient
{
    class CLogingWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CLogingWidget(QWidget* vParent = 0);
    };
}


#endif //LOGINGWIDGET_H
