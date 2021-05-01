#pragma once

#include <QWidget>

#include "protocol/command_table.h"

namespace Ui {
  class ChannelWidget;
}

class ChannelWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ChannelWidget(QWidget *parent = nullptr);
  ~ChannelWidget();

  void set_channel(const goodok::command::chat_t& datachat);

  QString channel_name() const;
  const goodok::command::messages_ptr history() const;

  void update_displayed_msg();

private:
  Ui::ChannelWidget *ui;
    goodok::command::messages_ptr history_;

};

