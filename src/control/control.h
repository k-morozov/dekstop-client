#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include "client/client.h"

#include "cache/history_cacher.h"

#include <vector>

// forward declaration
class LoginWidget;
class MainWindow;

/**
 * @brief Controller
 * @param ip and port server
 */
class Control: public QObject
{
    Q_OBJECT
public:
    Control(Storage::SqliteConfig config);

    void run_app(int argc, char** argv);
    /**
     * @brief Start communication with server
     * @param login user's login
     * @param password user's password
     * @param command request code that would have sent on opened connection
     * it could be AutorisationRequest or RegistrationRequest
     * @todo fix noreturn
     */
    void connect_to_server(const std::string& login, const std::string& password, goodok::command::TypeCommand command);

    /**
     * @brief Close client socket when destroy UI controller.
     */
    ~Control();

signals:
    /**
     * @brief Show reveived message
     */
    void send_text_to_gui(const goodok::command::msg_text_t&);

    void send_input_code(goodok::command::StatusCode);

public slots:
    /**
     * @brief User autorization
     * @param login user's login
     * @param password user's password
     * @todo fix typo autorisation -> authorization
     */
    void autorisation(const std::string& login, const std::string& password);

    /**
     * @brief User registration
     * @param login user's login
     * @param password user's password
     * @todo replace to async?
     */
    void registration(const std::string& login, const std::string& password);

    /**
     * @brief Send message
     * @param login sender's login
     * @param text message content
     * @param room_id sender's room
     */
    void get_text_from_gui(goodok::command::msg_text_t msg);

    /**
     * @brief Notify UI about received message
     * @param from sender's login
     * @param text message content
     * @param dt date and time of sending the text
     */
    void text_from_client(const goodok::command::msg_text_t& msg);


    /**
     * @brief change_window
     * @todo IWindow - classes
     */
    void change_window(goodok::command::StatusCode a_code);

    void join_to_channel(const std::string& channel_name);

    void channels_received(const std::vector<std::string>& channels);

    void joined(const std::string& channel_name);

    void history_received(const std::string& channel_name, const std::vector<goodok::command::msg_text_t>& hist);

private:
    std::shared_ptr<Client> client;
    Storage::HistoryCacher cacher;
    std::unique_ptr<LoginWidget> login_widget;
    std::unique_ptr<MainWindow> main_window;

    std::vector<uint8_t> __buffer;

    /**
     * @todo convert to 4 bytes
     */
    std::string ip = "127.0.0.1";
    int32_t port = goodok::command::SERVER_DEFAULT_PORT;
};

#endif // CONTROL_H
