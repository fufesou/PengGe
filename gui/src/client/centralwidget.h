/**
 * @file centralwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-04
 * @modified  Fri 2015-12-04 21:59:47 (+0800)
 */

#ifndef _CENTRALWIDGET_H
#define _CENTRALWIDGET_H

namespace GuiClient
{
    class CCentralWidget : public QTabWidget
    {
        Q_OBJECT

    public:
        explicit CCentralWidget(QWidget* vParent = 0);

    };
}

#endif //CENTRALWIDGET_H
