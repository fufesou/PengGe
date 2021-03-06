/**
 * @file displayitemwidget.h
 * @brief  
 * @author 298516439@qq.com, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-12-06
 * @modified  Tue 2015-12-08 00:41:44 (+0800)
 */

#ifndef DISPLAYITEMWIDGET_H
#define DISPLAYITEMWIDGET_H

#include  <QWidget>
#include  <QSharedPointer>

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QPushButton)

namespace GuiCommon
{
    QT_FORWARD_DECLARE_CLASS(CDisplayItemInfo)

    class CDisplayItemWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CDisplayItemWidget(const QSharedPointer<CDisplayItemInfo>& vItemInfo, QWidget* vParent = 0);

    signals:
        void doubleClicked(const QSharedPointer<CDisplayItemInfo>& voItemInfo);

    protected:
        void mouseDoubleClickEvent(QMouseEvent* vEvent) Q_DECL_OVERRIDE;

    private:
        void initWidget(void);

    private:
        QLabel* m_plbPortrait;
        QLabel* m_plbPeerName;
        QLabel* m_plbOtherInfo;
        QPushButton* m_pbtnInfo;

        QSharedPointer<CDisplayItemInfo> m_pItemInfo;
    };
}

#endif // DISPLAYITEMWIDGET_H
