#include "customtextedit.h"

#include <limits>
#include <QDebug>

CustomTextEdit::CustomTextEdit(const QString& text, QWidget* parent)
  : QTextEdit(text, parent), max_length_(std::numeric_limits<int>::max())
{
  connect(this, &CustomTextEdit::textChanged, this, &CustomTextEdit::text_changed);
}

CustomTextEdit::CustomTextEdit(QWidget* parent)
  : QTextEdit(parent), max_length_(std::numeric_limits<int>::max())
{
  connect(this, &CustomTextEdit::textChanged, this, &CustomTextEdit::text_changed);
}

void CustomTextEdit::set_max_length(int length) { max_length_ = length; }

int CustomTextEdit::max_length() const { return max_length_; }

void CustomTextEdit::text_changed()
{
  if (max_length_ < toPlainText().length()) {
    textCursor().deletePreviousChar();
  }
}
