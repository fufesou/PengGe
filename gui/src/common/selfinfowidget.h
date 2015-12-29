/**
 * @file selfinfowidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Tue 2015-12-08 00:43:34 (+0800)
 */

#ifndef SELFINFOWIDGET_H
#define SELFINFOWIDGET_H

#include <QWidget>

namespace GuiCommon
{
    class CSelfInfoWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CSelfInfoWidget(QWidget* vParent = 0);
        ~CSelfInfoWidget();

    private:
        void initWidget(void);
    };
}

#endif // SELFINFOWIDGET_H
