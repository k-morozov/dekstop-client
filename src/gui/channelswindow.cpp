#include "channelswindow.h"
#include "ui_channelswindow.h"
#include <iostream>
#include <string>

ChannelsWindow::ChannelsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelsWindow)
{
    ui->setupUi(this);

    QPalette pail;
    pail.setColor(this->backgroundRole(), Qt::white);
    this->setPalette(pail);

    ui->listView->show();
    m_channel_model = new ChannelsModel(this);
    ui->listView->setModel(m_channel_model);

    m_channel_delegate = new ChannelDelegate(this);
    ui->listView->setItemDelegate(m_channel_delegate);

    connect(ui->listView, SIGNAL(doubleClicked(QModelIndex)),this,SLOT(OnDoubleClicked(QModelIndex)));

    ui->room_id->setPlaceholderText("Enter channel to add");

    this->setWindowTitle("Channels");
}

void ChannelsWindow::set_client(std::shared_ptr<Client> a_client)
{
    client_ptr = a_client;
    connect(client_ptr.get(), &Client::channels_received, this, &ChannelsWindow::OnChannelsReceived);
}

void ChannelsWindow::update_list_channels() {
    m_channel_model->update();
}

ChannelsWindow::~ChannelsWindow()
{
    delete ui;
}

void ChannelsWindow::OnDoubleClicked(const QModelIndex &index) {
    client_ptr->set_current_channel(m_channel_model->get_channel(index.row()));
    emit sig_join_room(m_channel_model->get_channel(index.row()));
}

void ChannelsWindow::on_push_change_room_id_clicked()
{
    auto v_channel_name = ui->room_id->text();
    if (v_channel_name.isEmpty()) return;

    ui->room_id->clear();
    client_ptr->join_channel(v_channel_name.toStdString());

    // @todo add only success result
    auto ptr_to_history = client_ptr->get_ptr_history(v_channel_name.toStdString());
    m_channel_model->add_channel({v_channel_name.toStdString(), 1, 1, ptr_to_history});
}


void ChannelsWindow::OnChannelsReceived(const std::vector<std::string>& channels)
{
    for(const auto& channel : channels) {
        const auto ptr = client_ptr->get_ptr_history(channel);
        m_channel_model->add_channel({channel, 1, 1, ptr});
    }
}

