#include "channeldelegate.h"

#include "channeldelegate.h"
#include <QDebug>

ChannelDelegate::ChannelDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    groupIcon =  new QPixmap(":/theme/gui/images/chatwindow.jpg");
}

void ChannelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    chat_t item = index.model()->data(index).value<chat_t>();
    QStyleOptionViewItem myOpt = option;
    myOpt.displayAlignment = Qt::AlignLeft;

    const QRect &rect(myOpt.rect);
    painter->save();
    painter->setClipping(true);

    const QPalette &palette(myOpt.palette);
    QFont f(myOpt.font);

    painter->fillRect(rect, myOpt.state & QStyle::State_Selected ?
                             qRgb(230,230,230)  :
                             palette.light().color());
    painter->setFont(myOpt.font);
    int width = sizeHint(option,index).width();

    QFontMetrics font(f);
    QPixmap scaled = groupIcon->scaled(groupIconSize,
                                       Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    QBrush brush(scaled);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(brush);
    painter->translate(QPointF(rect.topLeft() + offset));
    painter->drawRoundedRect(QRect(QPoint(0,0), groupIconSize), groupIconRadius, groupIconRadius);
    painter->restore();

    painter->save();

    QRect GroupNameRect = myOpt.rect;
    GroupNameRect.setY(GroupNameRect.y() + textTopOffset);
    GroupNameRect.setX(groupIconSize.width() + textLeftOffset);
    GroupNameRect.setWidth(width - textRigthOffset);

    f.setBold(true);
    painter->setFont(f);
    painter->setPen(palette.text().color());
    painter->drawText(GroupNameRect, Qt::TextSingleLine,
                      font.elidedText(QString::fromStdString(item.name),
                      Qt::ElideRight,width - textRigthOffset));

    qDebug() << "channel name: " << item.name.data();
    int nameWidth = 0;
    if(item.history_ptr && !item.history_ptr->empty()){
        QFontMetrics metr(f);
        nameWidth = metr./*horizontalAdvance*/width(item.history_ptr->back().author.data());
        nameWidth += 5;
        QRect LastMessageNick = QRect(groupIconSize.width() + textLeftOffset,
                                   GroupNameRect.y() + lastMessageTopOffset,
                                      nameWidth,
                                      baseTextHeigth);

        painter->setPen(QColor(76,148,224));
        if (item.history_ptr) {
            painter->drawText(LastMessageNick, Qt::TextSingleLine,
                          font.elidedText(QString::fromStdString(item.history_ptr->back().author + ": "),
                                          Qt::ElideRight,width - textRigthOffset));
        }
    }

    f.setBold(false);
    QRect LastMessageRect = QRect(groupIconSize.width() + textLeftOffset + 5 + nameWidth,
                                  GroupNameRect.y() + lastMessageTopOffset,width - textRigthOffset,baseTextHeigth);
    painter->setFont(f);
    painter->setPen(palette.text().color());

    if (item.history_ptr && !item.history_ptr->empty()) {
        painter->drawText(LastMessageRect, Qt::TextSingleLine,
                          font.elidedText(QString::fromStdString(item.history_ptr->back().text),
                                          Qt::ElideRight,width - textRigthOffset));

        if(sizeHint(option,index).width() > timeMessageMaxOffset) {
            QRect TimeMessageRect = QRect(width - groupIconSize.width() - timeRigthOffset ,GroupNameRect.y(),
                                          timeMessageWidth,baseTextHeigth);
            painter->setFont(f);
            painter->setPen(palette.text().color());

            painter->drawText(TimeMessageRect, Qt::TextSingleLine, QString(get_time_to_gui(item.history_ptr->back().dt.time.hours,
                                                                                           item.history_ptr->back().dt.time.minutes).data()));
        }
    }
    painter->setPen(palette.mid().color());
    if(rect.y() != 0)
        painter->drawLine(GroupNameRect.x(),rect.y(),rect.width(),rect.y());

    painter->restore();
}

QSize ChannelDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    return QSize(option.widget->width() - (option.widget->contentsMargins().left() + option.widget->contentsMargins().right()),baseItemHeight);
}

