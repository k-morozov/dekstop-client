#include "chatmodel.h"

ChatModel::ChatModel(QObject *parent) : QAbstractListModel(parent)
{

}

void ChatModel::populateData(const std::deque<msg_text_t> & a_msgs)
{
    m_msgs.clear();
    m_msgs = a_msgs;
    return;
}

void ChatModel::add_msg(const msg_text_t& a_msg) {
    m_msgs.push_back(a_msg);
}

void ChatModel::update() {
    if (m_msgs.size()!=0) {
        emit dataChanged(index(static_cast<int>(m_msgs.size())-1, 0),
                         index(static_cast<int>(m_msgs.size())-1, 0));
    }
}

void ChatModel::clear() {
    m_msgs.clear();
    emit dataChanged(index(0, 0), index(static_cast<int>(m_msgs.size()), 0));
}

int ChatModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(m_msgs.size());
}

int ChatModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        msg_text_t value = m_msgs[static_cast<std::size_t>(index.row())];
        return QVariant::fromValue(value);
    }

    return QVariant();
}

QVariant ChatModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("My channels");
        }
    }
    return QVariant();
}
