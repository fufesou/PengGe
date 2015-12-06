#ifndef SELFINFOWIDGET_H
#define SELFINFOWIDGET_H

#include <QWidget>

namespace GuiCommon
{
    class CSelfInfoWidget : public QWidget
    {
        Q_OBJECT

    public:
        CSelfInfoWidget(QWidget* vParent = 0);
        ~CSelfInfoWidget();

    private:
        void initWidget(void);
    };
}

#endif // SELFINFOWIDGET_H
