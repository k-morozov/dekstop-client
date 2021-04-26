#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <queue>
#include <mutex>
#include <memory>
#include <set>
#include <unordered_map>
#include <boost/asio.hpp>
#include "client/msgtype.h"
#include <QWidget>
#include <QDebug>
#include <QTime>

#include "cache/sqlitecache.h"

/**
 * @brief Client class
 * 
 * @details Implement communication with server
 */
class Client: public QObject, public std::enable_shared_from_this<Client> {
   Q_OBJECT

public:
    /**
     * @brief Construct a new Client object
     * @param io boost::asio::io_service
     * @param eps 
     * @param request initial request the server
     */
    Client(boost::asio::io_service &io, const boost::asio::ip::tcp::resolver::results_type& _eps)
      : io_service(io), sock(io), eps(_eps)
    {
        qDebug() << "[" << QTime::currentTime().toString("hh:mm:ss.zzz") << "] " << "create client";
    }

    /**
     * @brief Start connection to the server
     */
    void do_connect(std::vector<uint8_t>&& __buffer);

    void join_channel(std::string);

    void send_msg_to_server(msg_text_t msg);


    auto get_login() const noexcept { return login; }
    void set_login(const std::string& new_login) { login = new_login; }
    auto get_password() const noexcept { return password; }
    void set_password(const std::string& new_password) { password = new_password; }
    auto get_channels() const noexcept {
        std::set<std::string> values;
        for(const auto& [channel, history] : m_channels_history) {
            values.insert(channel);
        }
        return values;
    }
    void set_current_channel(const std::string& a_channel_name) noexcept {
        m_channel_name = a_channel_name;
    }

    auto get_current_channel() const noexcept {
        return m_channel_name;
    }

    // @todo const and check limit?
    auto get_history(const std::string& a_channel_name) {
        return *m_channels_history[a_channel_name];
    }

    auto get_ptr_history(const std::string& a_channel_name) {
        if (auto it = m_channels_history.find(a_channel_name); it!=m_channels_history.end()) {
            return it->second;
        }
        else {
            auto [it2, flag] = m_channels_history.emplace(a_channel_name, std::make_shared<std::deque<msg_text_t>>());
            return it2->second;
        }

    }
    void set_login_id(identifier_t id)   { client_id = id;}

    void close_connection();

    ~Client() {
        std::cout << "Destr client" << std::endl;
        close_connection();
    }

    void send_history_request(const std::string& channel_name, DateTime since);
    void send_channels_request();

private:
    boost::asio::io_service &io_service;
    boost::asio::ip::tcp::socket sock;
    std::mutex mtx_sock;
    const boost::asio::ip::tcp::resolver::results_type& eps;

    std::vector<uint8_t> __read_buffer;
    std::array<uint8_t, Protocol::SIZE_HEADER> bin_buffer;
    std::queue<std::vector<uint8_t>> msg_to_server;

    std::string login;
    std::string password;
    identifier_t client_id;
    identifier_t current_room = 0;
    std::string m_channel_name;
    std::unordered_map<std::string, std::shared_ptr<std::deque<msg_text_t>>> m_channels_history;

private:

    void async_read_pb_header();
    void async_read_pb_msg(Serialize::Header);

    void do_read_pb_header(boost::system::error_code error, std::size_t nbytes);
    void do_read_input_response(boost::system::error_code, std::size_t);
    void do_read_reg_response(boost::system::error_code, std::size_t);
    void do_read_join_room_response(boost::system::error_code, std::size_t);
    void do_read_echo_response(boost::system::error_code, std::size_t);
    void do_read_history_response(boost::system::error_code, std::size_t);
    void do_read_channels_response(boost::system::error_code, std::size_t);

    void send_login_request(std::vector<uint8_t> && __buffer);

    void add_msg_to_send(std::vector<uint8_t> &&);
    void start_send_msgs();

signals:
    /**
     * @brief 
     * @param from sender's login
     * @param text 
     * @param dt date and time of sending the text
     */
    void send_text(msg_text_t);

    /**
     * @brief send input code
     */
    void send_input_code(StatusCode);

    void sig_update_channels();

    void joined(const std::string& channel_name);
    
    void history_received(const std::string& channel_name, const std::vector<msg_text_t>& hist);

    void channels_received(const std::vector<std::string>& channels);
};

#endif // CLIENT_H
