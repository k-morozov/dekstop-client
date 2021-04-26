#include "msgfactory.h"
#include <iostream>

#include <type_traits>

namespace Protocol {

[[nodiscard]]
ptr_header_t MsgFactory::create_header(TypeCommand command, std::size_t length) {
    auto header = std::make_unique<Serialize::Header>();
    header->set_length(length);

    header->set_version(1);
    header->set_command(static_cast<google::protobuf::int32>(command));

    return header;
}

[[nodiscard]]
std::vector<uint8_t> MsgFactory::serialize_response(ptr_header_t&& header_ptr, ptr_proto_response_t&& response_ptr) {
    std::vector<uint8_t> bin_buffer(SIZE_HEADER + response_ptr->ByteSizeLong());
    header_ptr->set_length(response_ptr->ByteSizeLong());
    header_ptr->SerializeToArray(bin_buffer.data(), SIZE_HEADER);
    response_ptr->SerializeToArray(bin_buffer.data() + SIZE_HEADER, static_cast<int>(response_ptr->ByteSizeLong()));

    return bin_buffer;
}

[[nodiscard]]
std::vector<uint8_t> MsgFactory::serialize_request(ptr_header_t&& header_ptr, ptr_proto_request_t&& request_ptr) {
    std::vector<uint8_t> bin_buffer(SIZE_HEADER + request_ptr->ByteSizeLong());
    header_ptr->set_length(request_ptr->ByteSizeLong());
    header_ptr->SerializeToArray(bin_buffer.data(), SIZE_HEADER);
    request_ptr->SerializeToArray(bin_buffer.data() + SIZE_HEADER, static_cast<int>(request_ptr->ByteSizeLong()));

    return bin_buffer;
}

msg_text_t MsgFactory::parse_text_req(Serialize::Request a_request) {
    msg_text_t result;
    if (a_request.has_text_request()) {
        result.author = a_request.text_request().login();
        result.channel_name = a_request.text_request().channel_name();
        result.text = a_request.text_request().text();

        // @todo need opt
        result.dt.date.day = a_request.text_request().datetime().day();
        result.dt.date.month = a_request.text_request().datetime().month();
        result.dt.date.year = a_request.text_request().datetime().year();
        result.dt.time.hours = a_request.text_request().datetime().hours();
        result.dt.time.minutes = a_request.text_request().datetime().minutes();
        result.dt.time.seconds = a_request.text_request().datetime().seconds();
    }

    return result;
}

msg_text_t MsgFactory::parse_text_res(Serialize::Response a_response) {
    msg_text_t result;
    if (a_response.has_text_response()) {
        result = parse_text_res(a_response.text_response());
    }

    return result;
}

msg_text_t MsgFactory::parse_text_res(const Serialize::TextResponse& a_response) {
    msg_text_t result;

    result.author = a_response.login();
    result.channel_name = a_response.channel_name();
    result.text = a_response.text();

    // @todo need opt
    result.dt.date.day = a_response.datetime().day();
    result.dt.date.month = a_response.datetime().month();
    result.dt.date.year = a_response.datetime().year();
    result.dt.time.hours = a_response.datetime().hours();
    result.dt.time.minutes = a_response.datetime().minutes();
    result.dt.time.seconds = a_response.datetime().seconds();

    return result;
}

ptr_datetime_t MsgFactory::create_datetime(const msg_text_t& msg) {
    auto dt = std::make_unique<Serialize::Datetime>();

    dt->set_hours(msg.dt.time.hours);
    dt->set_minutes(msg.dt.time.minutes);
    dt->set_seconds(msg.dt.time.seconds);
    dt->set_day(msg.dt.date.day);
    dt->set_month(msg.dt.date.month);
    dt->set_year(msg.dt.date.year);

    return dt;
}


// Response
[[nodiscard]]
ptr_proto_response_t MsgFactory::create_response(Traits::TypeCommandTag::Registration, int client_id) {
    auto reg_response = std::make_unique<Serialize::RegResponse>();

    reg_response->set_client_id(client_id);
    reg_response->set_msg_id(0);
    reg_response->set_chat_id(0);

    if (client_id != 0) {
        reg_response->set_status(Serialize::STATUS::OK);
    }
    else {
        reg_response->set_status(Serialize::STATUS::FAIL);
    }

    auto response = std::make_unique<Serialize::Response>();
    response->set_allocated_reg_response(reg_response.release());

    return response;
}

[[nodiscard]]
ptr_proto_response_t MsgFactory::create_response(Traits::TypeCommandTag::Authorisation, int client_id) {
    auto in_response = std::make_unique<Serialize::InputResponse>();

    in_response->set_client_id(client_id);
    in_response->set_msg_id(0);
    in_response->set_chat_id(0);

    if (client_id != 0) {
        in_response->set_status(Serialize::STATUS::OK);
    }
    else {
        in_response->set_status(Serialize::STATUS::FAIL);
    }

    auto response = std::make_unique<Serialize::Response>();
    response->set_allocated_input_response(in_response.release());

    return response;
}

[[nodiscard]]
ptr_proto_response_t MsgFactory::create_response(Traits::TypeCommandTag::Echo, const msg_text_t& msg) {
    auto text_response = std::make_unique<Serialize::TextResponse>();

    text_response->set_login(msg.author);
    text_response->set_channel_name(msg.channel_name);
    text_response->set_text(msg.text);

    auto dt = create_datetime(msg);
    text_response->set_allocated_datetime(dt.release());

    auto response = std::make_unique<Serialize::Response>();
    response->set_allocated_text_response(text_response.release());

    return response;
}

[[nodiscard]]
ptr_proto_response_t MsgFactory::create_response(Traits::TypeCommandTag::JoinRoom,
    const std::string& channel_name, bool flag) {
    
    auto jr_response = std::make_unique<Serialize::JoinRoomResponse>();
    jr_response->set_channel_name(channel_name);

    if (flag) jr_response->set_status(Serialize::STATUS::OK);
    else jr_response->set_status(Serialize::STATUS::FAIL);

    auto response = std::make_unique<Serialize::Response>();
    response->set_allocated_join_room_response(jr_response.release());

    return response;
}

[[nodiscard]]
ptr_proto_response_t MsgFactory::create_response(Traits::TypeCommandTag::History,
    const std::string& channel_name, const std::deque<msg_text_t>& messages) {
        
    auto history_response = std::make_unique<Serialize::HistoryResponse>();
    history_response->set_channel_name(channel_name);

    for (const auto& msg : messages) {
        // add a new message to the list and get a pointer to it
        auto text_response = history_response->add_messages();
        static_assert(std::is_same_v<decltype(text_response), Serialize::TextResponse*>);

        text_response->set_login(msg.author);
        text_response->set_channel_name(msg.channel_name);
        text_response->set_text(msg.text);
      
        auto dt = create_datetime(msg);
        text_response->set_allocated_datetime(dt.release());
    }

    auto response = std::make_unique<Serialize::Response>();
    response->set_allocated_history_response(history_response.release());

    return response;
}

[[nodiscard]]
ptr_proto_response_t MsgFactory::create_response(Traits::TypeCommandTag::Channels,
    const std::deque<std::string>& channels) {
    
    auto ch_response = std::make_unique<Serialize::ChannelsResponse>();
    for (const auto& channel : channels) {
      *ch_response->add_channels() = channel;
    }

    auto response = std::make_unique<Serialize::Response>();
    response->set_allocated_channels_response(ch_response.release());

    return response;
}


// Request
[[nodiscard]]
ptr_proto_request_t MsgFactory::create_request(Traits::TypeCommandTag::Registration,
    const std::string& login, const std::string& password) {

    auto in_request = std::make_unique<Serialize::RegRequest>();
    in_request->set_login(login);
    in_request->set_password(password);

    auto request = std::make_unique<Serialize::Request>();
    request->set_allocated_register_request(in_request.release());

    return request;
}

[[nodiscard]]
ptr_proto_request_t MsgFactory::create_request(Traits::TypeCommandTag::Authorisation,
    const std::string& login, const std::string& password) {

    auto in_request = std::make_unique<Serialize::InRequest>();
    in_request->set_login(login);
    in_request->set_password(password);

    auto request = std::make_unique<Serialize::Request>();
    request->set_allocated_input_request(in_request.release());

    return request;
}

[[nodiscard]]
ptr_proto_request_t MsgFactory::create_request(Traits::TypeCommandTag::Echo, const msg_text_t& msg) {
    auto text_request = std::make_unique<Serialize::TextRequest>();
    text_request->set_login(msg.author);
    text_request->set_channel_name(msg.channel_name);
    text_request->set_text(msg.text);

    auto dt = create_datetime(msg);
    text_request->set_allocated_datetime(dt.release());

    auto request = std::make_unique<Serialize::Request>();
    request->set_allocated_text_request(text_request.release());
    return request;
}

[[nodiscard]]
ptr_proto_request_t MsgFactory::create_request(Traits::TypeCommandTag::JoinRoom,
    identifier_t client_id, const std::string& channel_name) {

    auto jr_request = std::make_unique<Serialize::JoinRoomRequest>();
    jr_request->set_client_id(client_id);
    jr_request->set_channel_name(channel_name);

    auto request = std::make_unique<Serialize::Request>();
    request->set_allocated_join_room_request(jr_request.release());

    return request;
}

[[nodiscard]]
ptr_proto_request_t MsgFactory::create_request(Traits::TypeCommandTag::History,
    identifier_t client_id, const std::string& channel_name, DateTime since) {
    
    auto history_request = std::make_unique<Serialize::HistoryRequest>();
    history_request->set_client_id(client_id);
    history_request->set_channel_name(channel_name);

    auto dt = std::make_unique<Serialize::Datetime>();
    dt->set_hours(since.time.hours);
    dt->set_minutes(since.time.minutes);
    dt->set_seconds(since.time.seconds);
    dt->set_day(since.date.day);
    dt->set_month(since.date.month);
    dt->set_year(since.date.year);

    history_request->set_allocated_since(dt.release());

    auto request = std::make_unique<Serialize::Request>();
    request->set_allocated_history_request(history_request.release());

    return request;
}

[[nodiscard]]
ptr_proto_request_t MsgFactory::create_request(Traits::TypeCommandTag::Channels, identifier_t client_id)
{
    auto ch_request = std::make_unique<Serialize::ChannelsRequest>();
    ch_request->set_client_id(client_id);

    auto request = std::make_unique<Serialize::Request>();
    request->set_allocated_channels_request(ch_request.release());

    return request;
}


} // namespace Protocol


