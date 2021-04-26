#include "chatdelegate.h"

#include <QPainter>
#include <QAbstractItemView>

#include "mainwindow.h"

ChatDelegate::ChatDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

QSize ChatDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    msg_text_t item = get_message(index);
    QRect MessageRect = option.rect;
    MessageRect.setY( OffsetMsgFromLogin + option.rect.y());
    MessageRect.setX(textOffset);
    MessageRect.setWidth(option.widget->width() - 40);
    QFontMetrics fMetrics(option.font);
    int height = fMetrics.boundingRect(QRect(0, 0, option.widget->width() - MessageRect.x() - 35, 0),
                                       Qt::TextWrapAnywhere,
                                       QString::fromStdString(item.text)
                                       ).height();
    if(!item.text.empty()){
        height += 10;
    }
    if(height <= 20)
        return QSize(option.widget->width(),baseItemHeight);
    else {
         return QSize(option.widget->width(),height + 15);
    }
}

void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    msg_text_t item = get_message(index);
    QStyleOptionViewItem myOpt = option;
    myOpt.displayAlignment =  Qt::AlignVCenter;
    painter->save();
    painter->setClipping(true);

    QRect rect(myOpt.rect);
    rect.setWidth(option.widget->width());
    rect.setHeight(sizeHint(option,index).height());

    const QPalette &palette(myOpt.palette);
    QFont f(myOpt.font);
    painter->fillRect(rect, myOpt.state & QStyle::State_Selected ?
                             qRgb(230,230,230)  :
                             palette.light().color());

    painter->setFont(myOpt.font);

    int width = sizeHint(option,index).width();
    QFontMetrics font(f);

    painter->restore();

    painter->save();
    QRect NameRect = myOpt.rect;
    NameRect.setHeight(int(baseTextHeight));
    NameRect.setX(textOffset);
    NameRect.setWidth(width - rightMessageOffset);

    painter->setFont(f);
    painter->setPen(QColor(76,148,224));
    painter->drawText(NameRect, Qt::TextSingleLine,
                   QString(item.author.data()));

    QRect MessageRect = myOpt.rect;
    MessageRect.setY(MessageRect.y() + OffsetMsgFromLogin);
    MessageRect.setX(textOffset);
    MessageRect.setWidth(width - rightMessageOffset - 5);

    painter->setFont(f);
    painter->setPen(palette.text().color());
    painter->drawText(MessageRect,  Qt::AlignLeft | Qt::TextWrapAnywhere,
                   QString(item.text.data()));

    painter->restore();
    painter->save();

    QRect TimeRect = myOpt.rect;
    TimeRect.setHeight(static_cast<int>(baseTextHeight));
    TimeRect.setX(myOpt.widget->width() - 45);
    TimeRect.setWidth(width - rightMessageOffset);

    painter->drawText(TimeRect, Qt::TextSingleLine, QString(get_time_to_gui(item.dt.time.hours,
                                                                           item.dt.time.minutes).data()));
    painter->restore();
    painter->save();

    painter->restore();
}

void ChatDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const {
    QStyledItemDelegate::setEditorData(editor,index);
     QLabel* le= qobject_cast<QLabel*>(editor);
    if(le){
         QString text = get_message(index).author.data();
                le->setText(text);
        }

}

QWidget *ChatDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::createEditor(parent,option,index);
    QString text = QString::fromStdString(get_message(index).text);
    QLabel* editor = new QLabel(text,parent);
    return editor;
}

void ChatDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::updateEditorGeometry(editor,option,index);
    editor->setGeometry(option.rect);
}

msg_text_t ChatDelegate::get_message(const QModelIndex& index) const {
    const auto parent_ptr = qobject_cast<MainWindow*>(parent());
    const auto channel = parent_ptr->current_channel();
    const auto hist_ptr = parent_ptr->history(channel);
    return (*hist_ptr)[static_cast<size_t>(index.row())];
}
