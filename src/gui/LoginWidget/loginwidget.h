#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QStackedWidget>
#include <QWidget>
#include "client/client.h"

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

    void handler_input_code(goodok::command::StatusCode input_code);

signals:
    void send_autorisation_info(const std::string& login, const std::string& password);

    void send_registration_info(const std::string& login, const std::string& password);

private slots:
    void on_push_autorisation_clicked();

    void on_push_registration_clicked();


    void on_push_reg_main_clicked();

private:
    Ui::LoginWidget *ui;
    QWidget * m_log;
    QWidget * m_reg;
};

#endif // LOGINWIDGET_H
