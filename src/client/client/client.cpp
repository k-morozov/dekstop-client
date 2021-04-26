#include "client/client.h"

void Client::close_connection() {
    mtx_sock.lock();
    if(sock.is_open()) {
        boost::system::error_code ec;
        sock.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
        if (ec) {
            qWarning() << "Error when shutdown socket.";
        }
        sock.close(ec);
        if (ec) {
            qWarning() << "Error when close socket.";
        }
        qDebug() << "Close socket.";
    }
    mtx_sock.unlock();
}

void Client::do_connect(std::vector<uint8_t>&&  __buffer) {
    qDebug()<< "start do_connect()";

    boost::asio::async_connect(sock, eps,
        [this, ptr_buffer = std::move(__buffer)](boost::system::error_code ec, boost::asio::ip::tcp::endpoint) mutable {
           if (!ec) {
               send_login_request(std::move(ptr_buffer));
               qDebug() << "success finish do_connect()";
           }
           else {
               qWarning() << "error do_connect()";
           }
    });
}

void Client::send_login_request(std::vector<uint8_t>&& __buffer) {
//    qDebug() << "send_login_request()";

    boost::asio::async_write(sock, boost::asio::buffer(__buffer.data(), __buffer.size()),
                                     [this](boost::system::error_code ec, std::size_t nbytes) {
            if (!ec) {
                qDebug() << "send login_request=" << nbytes << " bytes";
                async_read_pb_header();

            } else {
                qWarning() << "error send_login_request(header)";
                close_connection();
            }
    });
}

void Client::send_history_request(const std::string& channel_name, DateTime since) {
    auto request_to_send = Protocol::MsgFactory::serialize<TypeCommand::HistoryRequest>(
        client_id, channel_name, since
    );

    add_msg_to_send(std::move(request_to_send));
}

void Client::send_channels_request() {
    auto request = Protocol::MsgFactory::serialize<TypeCommand::ChannelsRequest>(client_id);
    add_msg_to_send(std::move(request));
}

void Client::async_read_pb_header() {
//    qDebug() << "called async_read_pb_header()";
    boost::asio::async_read(sock, boost::asio::buffer(bin_buffer.data(), Protocol::SIZE_HEADER),
                            std::bind(&Client::do_read_pb_header,
                                      shared_from_this(),
                                      std::placeholders::_1,
                                      std::placeholders::_2));

}

void Client::do_read_pb_header(boost::system::error_code error, std::size_t) {
    if (!error) {
//        qDebug() << "new header read: " << nbytes << " bytes";

        Serialize::Header new_header;
        bool flag = new_header.ParseFromArray(bin_buffer.data(), Protocol::SIZE_HEADER);
        if (flag) {
            qDebug() << "parse header: OK";
        } else {
            qWarning() << "parse header: FAIL";
            return ;
        }
        async_read_pb_msg(new_header);

    } else {
        qWarning() << "error read_pb_request_header()";
        close_connection();
    }
}

void Client::async_read_pb_msg(Serialize::Header new_header) {
    __read_buffer.resize(new_header.length());
    switch (static_cast<TypeCommand>(new_header.command())) {
        case TypeCommand::AuthorisationResponse:
            qDebug()<< "AutorisationResponse:"  ;
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_input_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::RegistrationResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_reg_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::EchoRequest:
//            qDebug()<< "EchoRequest: "  ;
        break;
        case TypeCommand::EchoResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_echo_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::JoinRoomResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_join_room_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::HistoryResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_history_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        case TypeCommand::ChannelsResponse:
            boost::asio::async_read(sock, boost::asio::buffer(__read_buffer),
                                      std::bind(&Client::do_read_channels_response,
                                                shared_from_this(),
                                                std::placeholders::_1,
                                                std::placeholders::_2));
        break;
        default:
            qDebug()<< "Unknown command " << new_header.command();
            break;
    }
}

void Client::do_read_input_response(boost::system::error_code error, std::size_t nbytes) {
    if (!error) {
        qDebug()<< "read responser=" << nbytes << " bytes";

        Serialize::Response response;
        bool flag_parse = response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug() << "parse response: OK";
        } else {
            qWarning() << "parse response: FAIL";
            close_connection();
            return ;
        }

        if (response.has_input_response()) {
            qDebug()<< "response include input_response";
            if (response.input_response().status() == Serialize::STATUS::OK) {
                qDebug()<< "Autorisation response: OK";
                emit send_input_code(StatusCode::AutorOK);
            } else {
                emit send_input_code(StatusCode::IncorrectAutor);
                qWarning() << "Not found login/password";
                close_connection();
                return;
            }

            set_login_id(response.input_response().client_id());
//            change_room(current_room);

//            qDebug()<< "Success send_join_room_request()";
            async_read_pb_header();

        } else {
            qWarning() << "response not include input_response";
        }
    }
    else {
        qWarning() << "error when read response(data)";
        close_connection();
        return ;
    }
}

void Client::do_read_reg_response(boost::system::error_code error, std::size_t ) {
    if (!error) {
//        qDebug()<< "read responser=" << nbytes << " bytes";

        Serialize::Response response;
        bool flag_parse = response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug()<< "parse response: OK";
        } else {
            qWarning() << "parse response: FAIL";
            close_connection();
            return ;
        }

        if (response.has_reg_response()) {
            qDebug()<< "response include reg_response";
            if (response.reg_response().status() == Serialize::STATUS::OK) {
                qDebug()<< "Registration response: OK, client_id=" << response.reg_response().client_id();
                emit send_input_code(StatusCode::RegistrOK);
            } else {
                emit send_input_code(StatusCode::BusyRegistr);
                qWarning() << "Registration response: FAIL, busy login";
                close_connection();
                return;
            }

            set_login_id(response.reg_response().client_id());
//            change_room(current_room);

//            qDebug()<< "Success send_join_room_request()"  ;
            async_read_pb_header();

        } else {
            qWarning() << "response not include reg_response"  ;
        }
    }
    else {
        qWarning() << "error when read response(data)";
        close_connection();
        return ;
    }
}

void Client::do_read_join_room_response(boost::system::error_code error, std::size_t) {
    if (!error) {
//        qDebug()<< "read responser=" << nbytes << " bytes"  ;

        Serialize::Response response;
        bool flag_parse = response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug() << "parse response: OK";
        } else {
            qWarning() << "parse response: FAIL";
            close_connection();
            return ;
        }

        if (response.has_join_room_response()) {
            qDebug() << "response include join_room";
            if (response.join_room_response().status() == Serialize::STATUS::OK) {
                emit joined(response.join_room_response().channel_name());
            } else {
                qWarning() << "Response join_room: FAIL";
                close_connection();
                return;
            }
            async_read_pb_header();
        } else {
            qWarning() << "response not include join_room";
        }
    }
    else {
        qDebug()<< "error when read response(data)";
        close_connection();
        return ;
    }
}

void Client::do_read_echo_response(boost::system::error_code error, std::size_t) {
    std::cout << "do_read_echo_response()" << std::endl;
    if (!error) {
        Serialize::Response new_response;
        bool flag_parse = new_response.ParseFromArray(__read_buffer.data(), static_cast<int>(__read_buffer.size()));
        if (flag_parse) {
            qDebug()<< "parse echo_response: OK";
        } else {
            qWarning() << "parse echo_response: FAIL";
        }

        msg_text_t v_msg = Protocol::MsgFactory::parse_text_res(new_response);

        m_channels_history[v_msg.channel_name]->push_back(v_msg);
        
        std::cout << "from server " << v_msg.author << ": " << v_msg.text << std::endl;

        emit send_text(v_msg);

        async_read_pb_header();
    }
    else {
        qWarning() << "Error read_pb_text_res()";
        close_connection();
    }
}

void Client::do_read_history_response(boost::system::error_code error, std::size_t) {
    qDebug() << "do_read_history_response()";

    if (error) {
        qWarning() << "Error read_history_response()";
        close_connection();
        return;
    }

    Serialize::Response response;
    bool flag_parse = response.ParseFromArray(
        __read_buffer.data(), static_cast<int>(__read_buffer.size())
    );

    if (!flag_parse) {
        qDebug() << "Parse response: FAIL";
        close_connection();
        return;
    }

    qDebug() << "Parse response: OK";

    if (!response.has_history_response()) {
      qWarning() << "response not include history_response";
      close_connection();
      return;
    }

    qDebug() << "response include history_response";

    auto hr = response.history_response();
    const auto channel_name = hr.channel_name();
    
    std::vector<msg_text_t> hist;

    for (const auto& text_response : *hr.mutable_messages()) {
        const auto msg = Protocol::MsgFactory::parse_text_res(text_response);
        m_channels_history.try_emplace(channel_name, std::make_shared<std::deque<msg_text_t>>());
        m_channels_history[channel_name]->push_back(msg);
        hist.push_back(msg);
    }

    emit history_received(channel_name, hist);

    emit sig_update_channels();

    async_read_pb_header();
}

void Client::do_read_channels_response(boost::system::error_code error, std::size_t) {
    qDebug() << "do_read_channels_response()";

    if (error) {
        qWarning() << "Error read_channels_response()";
        close_connection();
        return;
    }

    Serialize::Response response;
    bool flag_parse = response.ParseFromArray(
        __read_buffer.data(), static_cast<int>(__read_buffer.size())
    );

    if (!flag_parse) {
        qDebug() << "Parse response: FAIL";
        close_connection();
        return;
    }

    qDebug() << "Parse response: OK";

    if (!response.has_channels_response()) {
        qWarning() << "response not include channels_response";
        close_connection();
        return;
    }

    qDebug() << "response include channels_response";

    auto cr = response.channels_response();

    std::vector<std::string> channels;

    std::copy(cr.channels().begin(), cr.channels().end(), std::back_inserter(channels));

    emit channels_received(channels);

    async_read_pb_header();
}

void Client::add_msg_to_send(std::vector<uint8_t>&& a_request_ptr) {
    bool process_write = !msg_to_server.empty();
    msg_to_server.push(std::move(a_request_ptr));

    if (!process_write) {
        start_send_msgs();
    }
}

void Client::start_send_msgs() {
    boost::asio::async_write(sock, boost::asio::buffer(msg_to_server.front()),
        [this](boost::system::error_code ec, std::size_t) {
        if (!ec) {
                msg_to_server.pop();
                if (!msg_to_server.empty()) {
                    start_send_msgs();
                }
        }
        else {
            qWarning() << "Error send_request_header()";
            close_connection();
        }
    });
}

void Client::join_channel(std::string a_new_channel) {
    qDebug() << "called Client::join_channel: " << a_new_channel.data() 
                << ", from client_id=" << client_id;
    
    auto v_bin_buffer = Protocol::MsgFactory::serialize<TypeCommand::JoinRoomRequest>(
        client_id, a_new_channel
    );
    add_msg_to_send(std::move(v_bin_buffer));
}

void Client::send_msg_to_server(msg_text_t a_msg) {
    auto a_bin_buffer = Protocol::MsgFactory::serialize<TypeCommand::EchoRequest>(a_msg);
    add_msg_to_send(std::move(a_bin_buffer));
}

