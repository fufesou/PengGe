/**
 * @file mainwidget.h
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  ���� 2015-12-03 21:04:40 �й���׼ʱ��
 */

#ifndef _MAINWIDGET_H
#define _MAINWIDGET_H

#include  <QWidget>

namespace GuiClient
{
    class CMainWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CMainWidget(QWidget* vParent = 0);

    private:
        void initWidgets(void);

    private:
        QWidget* m_pSelfInfoWidget;
        QWidget* m_pMainDisplayWidget;
        QWidget* m_pCtrlItemsWidget;
    };
}


#endif //MAINWIDGET_H
