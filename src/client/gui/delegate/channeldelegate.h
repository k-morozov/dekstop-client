#ifndef CHANNELDELEGATE_H
#define CHANNELDELEGATE_H

#include "gui/models/channelsmodel.h"
#include <QStyledItemDelegate>
#include <QPainter>


class ChannelDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChannelDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const override;

private:
    QPixmap *groupIcon;
    QPoint offset{5,5};
    int textLeftOffset = 15;
    int textTopOffset = 5;
    int baseTextHeigth = 20;
    int lastMessageTopOffset = 30;
    int textRigthOffset = 150;
    int timeMessageWidth = 40;
    int timeMessageMaxOffset = 140;
    int baseItemHeight = 60;
    int timeRigthOffset = 20;
    QSize groupIconSize {25,25};
    int groupIconRadius = 50;
};

#endif // CHANNELDELEGATE_H
