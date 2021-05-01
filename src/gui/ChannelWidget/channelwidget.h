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

  void set_channel(const chat_t& datachat);

  QString channel_name() const;
  const messages_ptr history() const;

  void update_displayed_msg();

private:
  Ui::ChannelWidget *ui;
  messages_ptr history_;

};

