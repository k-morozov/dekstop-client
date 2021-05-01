#pragma once

#include <QMainWindow>

#include "client/client.h"
#include "ChannelWidget/channelwidget.h"

class ChatDelegate;

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void set_login(const std::string& login);
  std::string current_channel() const;
    goodok::command::messages_ptr history(const std::string& channel_name);

signals:
  void join_to_channel(const std::string&);
  void send_message(const goodok::command::msg_text_t&);

public slots:
  void channel_received(const std::string& channel, goodok::command::messages_ptr history);
  void history_received(const std::string& channel);
  void message_received(const goodok::command::msg_text_t& msg);
  void joined(const std::string& channel_name, goodok::command::messages_ptr history);

private slots:
  void channel_selected();
  void join_clicked();
  void send_clicked();

private:
  Ui::MainWindow *ui;
  std::string login_;
  std::unordered_map<std::string, std::unique_ptr<ChannelWidget>> channels_;
  std::unique_ptr<ChatDelegate> chat_delegate_;

  void add_channel(goodok::command::chat_t);
  void add_message(/*msg_text_t*/);
};

