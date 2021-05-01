#include "channelsmodel.h"

ChannelsModel::ChannelsModel(QObject *parent) : QAbstractListModel(parent)
{

}

void ChannelsModel::populateData(const QList<chat_t> & channels)
{
    listChannels.clear();
    listChannels = channels;
    return;
}

void ChannelsModel::add_channel(chat_t a_msg) {
    listChannels.append(a_msg);
    update();
}

void ChannelsModel::update() {
    emit dataChanged(index(listChannels.size()-1, 0), index(listChannels.size()-1, 0));
}

int ChannelsModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return listChannels.length();
}

int ChannelsModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 1;
}

QVariant ChannelsModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    if (index.column() == 0) {
        auto str = "channel_id = " + listChannels[index.row()].name;
//        return QString(str.data());

        chat_t value = listChannels[index.row()];
        return QVariant::fromValue(value);
    }

    return QVariant();
}

QVariant ChannelsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section == 0) {
            return QString("My channels");
        }
    }
    return QVariant();
}

