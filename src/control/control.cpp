#include "control/control.h"

#include "gui/LoginWidget/loginwidget.h"
#include "gui/mainwindow.h"

#include <QTextCursor>

Control::Control(Storage::SqliteConfig config)
  : cacher(Storage::SqliteCache{config})
  , login_widget(std::make_unique<LoginWidget>())
  , main_window(std::make_unique<MainWindow>())
{
    qDebug() << "Ctor Control";

    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<goodok::command::StatusCode>("StatusCode");
    qRegisterMetaType<goodok::command::StatusCode>("goodok::command::StatusCode");
    qRegisterMetaType<goodok::DateTime>("DateTime");
    qRegisterMetaType<goodok::command::ClientTextMsg>("ClientTextMsg");

    connect(login_widget.get(), &LoginWidget::send_autorisation_info, this, &Control::autorisation);
    connect(login_widget.get(), &LoginWidget::send_registration_info, this, &Control::registration);

    connect(this, &Control::send_text_to_gui, main_window.get(), &MainWindow::message_received);
    connect(main_window.get(), &MainWindow::send_message, this, &Control::get_text_from_gui);
    connect(main_window.get(), &MainWindow::join_to_channel, this, &Control::join_to_channel);
}

void Control::run_app(int argc, char** argv) {
    qDebug() << "Control::run_app";

    if (argc>1) {
        ip = std::string(argv[1]);
        if (argc>2) {
            port = std::stoi(argv[2]);
        }
    }

    login_widget->show();
}

void Control::connect_to_server(const std::string& login, const std::string& password, goodok::command::TypeCommand command) {
    qDebug() <<  "connect_to_server()";

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    auto endpoints = resolver.resolve(ip, std::to_string(port));

    std::vector<uint8_t> bin_buffer;

    switch (command) {
    case goodok::command::TypeCommand::RegistrationRequest:
        bin_buffer = goodok::MsgFactory::serialize<goodok::command::TypeCommand::RegistrationRequest>(login, password);
        break;
    case goodok::command::TypeCommand::AuthorisationRequest:
        bin_buffer = goodok::MsgFactory::serialize<goodok::command::TypeCommand::AuthorisationRequest>(login, password);
        break;
    default: break;
    }

    client = std::make_shared<Client>(io_service, endpoints);

    connect(client.get(), &Client::send_text, this, &Control::text_from_client);
    connect(client.get(), &Client::send_input_code, this, &Control::change_window);
    connect(client.get(), &Client::channels_received, this, &Control::channels_received);
    connect(client.get(), &Client::history_received, this, &Control::history_received);
    connect(client.get(), &Client::joined, this, &Control::joined);

    cacher.set_client(client);

    client->do_connect(std::move(bin_buffer));
    client->set_login(login);
    main_window->set_login(login);

    std::thread th([&io_service]() {
        io_service.run();
    });
    th.join();

    if (client) {
        client->close_connection();
    }

    qDebug() << "close connect_to_server";
}

Control::~Control() {
    qDebug() << "Destr Control" ;
    if (client) {
        client->close_connection();
    }
}

void Control::autorisation(const std::string& login, const std::string& password) {
    std::thread th([this, login, password]() {
        try {
            connect_to_server(login, password, goodok::command::TypeCommand::AuthorisationRequest);
        } catch (std::exception &ex) {
             qDebug() << "exception from thread: " << ex.what();
        }
    });
    th.detach();
}

void Control::registration(const std::string& login, const std::string& password) {
    std::thread th([this, login, password]() {
        try {
            connect_to_server(login, password, goodok::command::TypeCommand::RegistrationRequest);
        } catch (std::exception &ex) {
             qDebug() << "exception from thread: " << ex.what();
        }
    });
    th.detach();
}

void Control::get_text_from_gui(goodok::command::msg_text_t msg) {
    qDebug() << "send msg to server: " << msg.dt.time.hours << ":" << msg.dt.time.minutes ;
    client->send_msg_to_server(msg);
}

void Control::text_from_client(const goodok::command::msg_text_t& msg) { emit send_text_to_gui(msg); }

void Control::change_window(goodok::command::StatusCode a_code) {
    if (a_code == goodok::command::StatusCode::BusyRegistr || a_code == goodok::command::StatusCode::IncorrectAutor) {
        login_widget->handler_input_code(a_code);
    }
    else {
        login_widget->hide();
        main_window->show();
    }
}

void Control::join_to_channel(const std::string& channel_name)
{
  client->join_channel(channel_name);
}

void Control::channels_received(const std::vector<std::string>& channels)
{
  for (const auto& channel : channels) {
    const auto ptr = client->get_ptr_history(channel);
    main_window->channel_received(channel, ptr);
  }
}

void Control::joined(const std::string& channel_name)
{
  const auto ptr = client->get_ptr_history(channel_name);
  main_window->joined(channel_name, ptr);
}

void Control::history_received(const std::string& channel_name, const std::vector<goodok::command::msg_text_t>& )
{
  main_window->history_received(channel_name);
}

