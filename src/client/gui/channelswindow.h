#ifndef CHANNELSWINDOW_H
#define CHANNELSWINDOW_H

#include <QWidget>
#include <QtWidgets>

#include "client/client.h"
#include "gui/delegate/channeldelegate.h"

namespace Ui {
class ChannelsWindow;
}

class ChannelsWindow : public QWidget
{
    Q_OBJECT

public:
    ChannelsWindow(QWidget *parent = nullptr);
    ~ChannelsWindow();

    void set_client(std::shared_ptr<Client> a_client);

    void update_list_channels();

signals:
    void sig_add_channel(std::string);
    void sig_join_room(std::string);
public slots:
    void OnDoubleClicked(const QModelIndex &index);
    void OnChannelsReceived(const std::vector<std::string>& channels);
private slots:
    void on_push_change_room_id_clicked();

private:
    Ui::ChannelsWindow *ui;
    std::shared_ptr<Client> client_ptr;
    ChannelsModel *m_channel_model;

    ChannelDelegate *m_channel_delegate;
};

#endif // CHANNELSWINDOW_H
