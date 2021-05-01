#ifndef APPCHANNELSMODEL_H
#define APPCHANNELSMODEL_H

#include <QAbstractListModel>
#include "client/msgtype.h"

class ChannelsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ChannelsModel(QObject *parent = 0);

    void populateData(const QList<chat_t> &contactName);
    void add_channel(chat_t);
    void update();

    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    std::string get_channel(int index) {
        return listChannels.at(index).name;
    }
private:
    QList<chat_t> listChannels;


};

#endif // APPCHANNELSMODEL_H
