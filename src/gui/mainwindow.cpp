#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "MessageWidget/messagewidget.h"
#include "CustomTextEdit/customtextedit.h"
#include "delegate/chatdelegate.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , channels_()
  , chat_delegate_(std::make_unique<ChatDelegate>(this))
{
  ui->setupUi(this);

  ui->lwMessages->setItemDelegate(chat_delegate_.get());

  ui->teMessage->hide();
  ui->bSend->hide();

  ui->teMessage->set_max_length(goodok::command::Block::TextMessage);

  connect(ui->lwChannels, &QListWidget::itemSelectionChanged, this, &MainWindow::channel_selected);
  connect(ui->bJoin, &QPushButton::clicked, this, &MainWindow::join_clicked);
  connect(ui->bSend, &QPushButton::clicked, this, &MainWindow::send_clicked);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::set_login(const std::string &login)
{
  login_ = login;
}

std::string MainWindow::current_channel() const
{
  return ui->lChannelName->text().toStdString();
}

goodok::command::messages_ptr MainWindow::history(const std::string& channel_name)
{
  return channels_[channel_name]->history();
}

void MainWindow::channel_received(const std::string& channel, goodok::command::messages_ptr history)
{
  add_channel({channel, 1, 1, history});
}

void MainWindow::history_received(const std::string& channel)
{
  channels_[channel]->update_displayed_msg();
}

void MainWindow::channel_selected()
{
  const auto item = ui->lwChannels->currentItem();
  const auto widget = ui->lwChannels->itemWidget(item);
  const auto channel_widget = qobject_cast<ChannelWidget*>(widget);

  ui->lChannelName->setText(channel_widget->channel_name());

  ui->lwMessages->clear();

  for (const auto& msg : *channel_widget->history()) {
    Q_UNUSED(msg)
    add_message();
  }

  ui->teMessage->show();
  ui->bSend->show();
}

void MainWindow::join_clicked()
{
  const auto channel_name = ui->leSearch->text().toStdString();

  if (channel_name.empty())
    return;

  ui->leSearch->clear();
  emit join_to_channel(channel_name);
}

void MainWindow::send_clicked()
{
  const auto text = ui->teMessage->toPlainText().toStdString();

  if (text.empty())
    return;

    goodok::command::msg_text_t msg {
    login_,                                 // Author
    text,                                   // Text
    ui->lChannelName->text().toStdString(), // Channel name
    get_current_DateTime()                  // Timestamp
  };

  emit send_message(msg);

  ui->teMessage->clear();
}

void MainWindow::joined(const std::string& channel_name, goodok::command::messages_ptr history)
{
  add_channel({channel_name, 1, 1, history});
}

void MainWindow::message_received(const goodok::command::msg_text_t& msg)
{
  const auto it = channels_.find(msg.channel_name);
  it->second->update_displayed_msg();

  if (ui->lChannelName->text().toStdString() == it->first) {
    add_message();
  }
}

void MainWindow::add_channel(goodok::command::chat_t datachat)
{
  channels_[datachat.name] = std::make_unique<ChannelWidget>();
  channels_[datachat.name]->set_channel(datachat);

  auto item = new QListWidgetItem(ui->lwChannels);
  const QSize size_hint {
    ui->lwChannels->width(),
    channels_[datachat.name]->sizeHint().height()
  };
  item->setSizeHint(size_hint);

  ui->lwChannels->setItemWidget(item, channels_[datachat.name].get());
}

void MainWindow::add_message(/*msg_text_t msg*/)
{
  const auto item = new QListWidgetItem(ui->lwMessages);
  Q_UNUSED(item)
}

