#ifndef CHATMODEL_H
#define CHATMODEL_H

#include <deque>
#include "client/msgtype.h"

#include <QAbstractListModel>
#include <QDebug>


class ChatModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ChatModel(QObject *parent = 0);

    void populateData(const std::deque<msg_text_t> &);
    void add_msg(const msg_text_t&);
    void clear();
    void update();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    std::deque<msg_text_t> m_msgs;

};

#endif // CHATMODEL_H
