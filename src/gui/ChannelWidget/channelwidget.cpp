#include "channelwidget.h"
#include "ui_channelwidget.h"

#include <sstream>
#include <iomanip>

ChannelWidget::ChannelWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ChannelWidget)
  , history_(nullptr)
{
  ui->setupUi(this);
}

ChannelWidget::~ChannelWidget()
{
  delete ui;
}

void ChannelWidget::set_channel(const goodok::command::chat_t& datachat)
{
  history_ = datachat.history_ptr;
  ui->lChannelName->setText(QString::fromStdString(datachat.name));
  update_displayed_msg();
}

QString ChannelWidget::channel_name() const
{
  return ui->lChannelName->text();
}

const goodok::command::messages_ptr ChannelWidget::history() const
{
  return history_;
}

void ChannelWidget::update_displayed_msg()
{
  if (!history_ || history_->empty())
    return;

  const auto& last_message = history_->back();

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << last_message.dt.date.day << '.'
      << std::setfill('0') << std::setw(2) << last_message.dt.date.month << '.'
      << std::setfill('0') << std::setw(4) << last_message.dt.date.year << '\n'
      << std::setfill('0') << std::setw(2) << last_message.dt.time.hours << ':'
      << std::setfill('0') << std::setw(2) << last_message.dt.time.minutes << ':'
      << std::setfill('0') << std::setw(2) << last_message.dt.time.seconds;

  ui->lLastMessage->setText(QString::fromStdString(last_message.author + ": " + last_message.text));
  ui->lTimestamp->setText(QString::fromStdString(oss.str()));
}
