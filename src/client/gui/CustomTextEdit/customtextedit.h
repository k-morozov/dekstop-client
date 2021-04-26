#pragma once

#include <QTextEdit>

class CustomTextEdit : public QTextEdit
{
public:
  explicit CustomTextEdit(const QString& text, QWidget* parent = nullptr);
  explicit CustomTextEdit(QWidget* parent = nullptr);

  void set_max_length(int length);
  int max_length() const;

protected slots:
  void text_changed();

protected:
  int max_length_;
};

