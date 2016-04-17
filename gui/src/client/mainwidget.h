/**
 * @file mainwidget.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-03
 * @modified  Tue 2015-12-08 00:35:43 (+0800)
 */

#ifndef _MAINWIDGET_H
#define _MAINWIDGET_H

#include  <QWidget>

namespace GuiCommon
{
    QT_FORWARD_DECLARE_CLASS(CDisplayItemInfo)
}

namespace GuiClient
{
    class CMainWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CMainWidget(QWidget* vParent = 0);

    signals:
        void beginChat(const GuiCommon::CDisplayItemInfo& vPeerInfo);
        void logout(void);

    private:
        void initWidget(void);

    private:
        QWidget* m_pSelfInfoWidget;
        QWidget* m_pMainDisplayWidget;
        QWidget* m_pCtrlItemsWidget;
    };
}


#endif //MAINWIDGET_H
