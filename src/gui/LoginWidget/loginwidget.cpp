#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QMessageBox>

LoginWidget::LoginWidget(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    m_log = new QWidget(this);
    m_reg = new QWidget(this);
    ui->logon->setFocus();

    m_log->setLayout(ui->hello_layout);
    m_reg->setLayout(ui->reg_layout);
    this->addWidget(m_log);
    this->addWidget(m_reg);
    this->setCurrentWidget(m_log);
    this->resize(400,550);
    this->setMinimumSize(250,350);
    this->setMaximumSize(250,350);

    this->setWindowTitle("GoodOK");
    QPalette pail;
    pail.setColor(this->backgroundRole(), Qt::white);
    this->setPalette(pail);

    ui->password->setEchoMode(QLineEdit::Password);
    ui->reg_psw->setEchoMode(QLineEdit::Password);
    ui->reg_psw_retry->setEchoMode(QLineEdit::Password);

    QWidget::setTabOrder(ui->logon, ui->password);
    QWidget::setTabOrder(ui->password, ui->push_autorisation);
    QWidget::setTabOrder(ui->push_autorisation, ui->push_registration);
    QWidget::setTabOrder(ui->push_registration, ui->logon);

    ui->logon->setPlaceholderText("Enter login");
    ui->password->setPlaceholderText("Enter password");

    ui->reg_logon->setPlaceholderText("Enter your login");
    ui->reg_psw->setPlaceholderText("Enter your password");
    ui->reg_psw_retry->setPlaceholderText("Entry your password");
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_push_registration_clicked()
{
    this->setCurrentWidget(m_reg);
}

void LoginWidget::handler_input_code(goodok::command::StatusCode code) {
    ui->logon->clear();
    ui->password->clear();

    switch (code) {
        case goodok::command::StatusCode::RegistrOK:
            QMessageBox::information(this, "registration", "You successfully registered.");
        break;

        case goodok::command::StatusCode::BusyRegistr:
            QMessageBox::information(this, "error from server", "this login is already registered.");
        break;

        case goodok::command::StatusCode::AutorOK:
        break;

        case goodok::command::StatusCode::IncorrectAutor:
            QMessageBox::information(this, "error from server", "check login/password");
        break;
    default:
        break;
    }
}

void LoginWidget::on_push_autorisation_clicked()
{
    const auto login = ui->logon->text().toStdString();
    const auto password = ui->password->text().toStdString();
    if (login.empty() || password.empty()) {
        QMessageBox::information(this, "error", "enter correct information");
        return;
    }
    send_autorisation_info(login, password);
    ui->logon->clear();
    ui->password->clear();
}

void LoginWidget::on_push_reg_main_clicked()
{
    const auto a_login = ui->reg_logon->text().toStdString();
    const auto a_password = ui->reg_psw->text().toStdString();
    const auto a_password_retry = ui->reg_psw_retry->text().toStdString();
    if (a_login.empty() || a_password.empty() || a_password != a_password_retry) {
        QMessageBox::information(this, "error", "enter correct information");
        return;
    }
    send_registration_info(a_login, a_password);
    ui->reg_logon->clear();
    ui->reg_psw->clear();
    ui->reg_psw_retry->clear();
}
