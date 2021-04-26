#include "messagewidget.h"
#include "ui_messagewidget.h"

#include <sstream>
#include <iomanip>

MessageWidget::MessageWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::MessageWidget)
{
  ui->setupUi(this);
}

MessageWidget::~MessageWidget()
{
  delete ui;
}

void MessageWidget::set_message(const msg_text_t& msg)
{
  ui->lAuthor->setText(QString::fromStdString(msg.author));
  ui->teText->setText(QString::fromStdString(msg.text));

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << msg.dt.date.day << '.'
      << std::setfill('0') << std::setw(2) << msg.dt.date.month << '.'
      << std::setfill('0') << std::setw(4) << msg.dt.date.year << '\n'
      << std::setfill('0') << std::setw(2) << msg.dt.time.hours << ':'
      << std::setfill('0') << std::setw(2) << msg.dt.time.minutes << ':'
      << std::setfill('0') << std::setw(2) << msg.dt.time.seconds;

  ui->lTimestamp->setText(QString::fromStdString(oss.str()));
}
