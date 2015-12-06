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
