#pragma once

#include <QWidget>

#include "protocol/command_table.h"

namespace Ui {
  class MessageWidget;
}

class MessageWidget : public QWidget
{
  Q_OBJECT

public:
  explicit MessageWidget(QWidget *parent = nullptr);
  ~MessageWidget();

  void set_message(const goodok::command::msg_text_t& msg);

private:
  Ui::MessageWidget *ui;
};

