#ifndef COMMAND_TABLE_H
#define COMMAND_TABLE_H

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <deque>
#include <cassert>

#include "datetime.h"

constexpr uint32_t PROTOCOL_VERS = 2;
constexpr unsigned short SERVER_DEFAULT_PORT = 7777;

using identifier_t = int32_t;

namespace Block {
    constexpr uint32_t VersionProtocol   = 2;
    constexpr uint32_t Command           = 2;
    constexpr uint32_t Datetime          = sizeof(DateTime);
    constexpr uint32_t LoginName         = 16;
    constexpr uint32_t Password          = 32;
    constexpr uint32_t LoginId           = sizeof(identifier_t);
    constexpr uint32_t RoomId            = sizeof(identifier_t);
    constexpr uint32_t TextMessage       = 255;

    constexpr uint32_t Header            = VersionProtocol + Command + Datetime;
    constexpr uint32_t InputOption       = LoginName + Password;
}

enum class TypeCommand : uint16_t {
    Unknown                     = 0xFFFF,
    RegistrationRequest         = 0x0001,
    RegistrationResponse        = 0x0002,
    AuthorisationRequest        = 0x0003,
    AuthorisationResponse       = 0x0004,
    EchoRequest                 = 0x0005,
    EchoResponse                = 0x0006,
    JoinRoomRequest             = 0x0007,
    JoinRoomResponse            = 0x0008,
    HistoryRequest              = 0x0009,
    HistoryResponse             = 0x000A,
    ChannelsRequest             = 0x000B,
    ChannelsResponse            = 0x000C
};

static const std::unordered_map< TypeCommand, std::string> type_command_str = {
    {TypeCommand::RegistrationRequest, "RegistrationRequest"},
    {TypeCommand::RegistrationResponse, "RegistrationResponse"},
    {TypeCommand::AuthorisationRequest, "AuthorisationRequest"},
    {TypeCommand::AuthorisationResponse, "AutorisationResponse"},
    {TypeCommand::EchoRequest, "EchoRequest"},
    {TypeCommand::EchoResponse, "EchoResponse"},
    {TypeCommand::JoinRoomRequest, "JoinRoomRequest"},
    {TypeCommand::JoinRoomResponse, "JoinRoomResponse"},
    {TypeCommand::HistoryRequest, "HistoryRequest"},
    {TypeCommand::HistoryResponse, "HistoryResponse"},
    {TypeCommand::ChannelsRequest, "ChannelsRequest"},
    {TypeCommand::ChannelsResponse, "ChannelsRespone"},

    {TypeCommand::Unknown, "Unknown"}
};
inline const std::string& get_command_str(TypeCommand command) {
    if (auto it=type_command_str.find(command); it!=type_command_str.end()) {
        return it->second;
    }
    assert(type_command_str.at(TypeCommand::Unknown)=="Unknown");
    return type_command_str.at(TypeCommand::Unknown);
}

struct TextSendData {
    std::string channel_name;
    std::string login;
    std::string text;
};

enum class StatusCode : int {
    RegistrOK = 1,
    AutorOK,
    BusyRegistr,
    IncorrectAutor
};

struct ClientTextMsg {
    std::string author;
    std::string text;
    std::string channel_name;
    DateTime dt;
};

inline bool operator==(const ClientTextMsg& lhs, const ClientTextMsg& rhs) {
    return lhs.author == rhs.author && 
        lhs.text == rhs.text &&
        lhs.channel_name == rhs.channel_name &&
        lhs.dt == rhs.dt;
}

using msg_text_t = ClientTextMsg;
using messages_ptr = std::shared_ptr<std::deque<msg_text_t>>;

struct DataChat {
    std::string name;
    identifier_t id;
    int count_subs = 0;

    messages_ptr history_ptr;
};
using chat_t = DataChat;
#endif // COMMAND_TABLE_H
