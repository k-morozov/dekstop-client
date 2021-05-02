#ifndef CHATDELEGATE_H
#define CHATDELEGATE_H


#include <QStyledItemDelegate>
#include <QTextEdit>
#include <QPainter>
#include <QDebug>
#include <QLabel>

#include "protocol/protocol.h"

Q_DECLARE_METATYPE(std::string)

class ChatDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChatDelegate(QWidget *parent = 0);

    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const;
    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;

    void setEditorData(QWidget* editor, const QModelIndex &index) const ;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QPoint offset{5,5};
    int textOffset = 5;
    int OffsetMsgFromLogin = 14;
    int baseItemHeight = 50;
    int rightMessageOffset = 10;
    float baseTextHeight = 20;

    goodok::command::msg_text_t get_message(const QModelIndex& index) const;
};
#endif // CHATDELEGATE_H
