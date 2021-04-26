#pragma once

#include "command_table.h"

namespace Traits::TypeCommandTag {

struct Unknow {};

struct Request {};
struct Response {};

struct Registration {};
struct Authorisation {};
struct Echo {};
struct JoinRoom {};
struct History {};
struct Channels {};

} // namespace Traits::TypeCommandTag

namespace Traits {

template <TypeCommand T>
struct TypeCommandTraits
{
  using Category = TypeCommandTag::Unknow;
  using Value = TypeCommandTag::Unknow;
};


template <>
struct TypeCommandTraits<TypeCommand::RegistrationResponse>
{
  using Category = TypeCommandTag::Response;
  using Value = TypeCommandTag::Registration;
};

template <>
struct TypeCommandTraits<TypeCommand::RegistrationRequest>
{
  using Category = TypeCommandTag::Request;
  using Value = TypeCommandTag::Registration;
};

template <>
struct TypeCommandTraits<TypeCommand::AuthorisationResponse>
{
  using Category = TypeCommandTag::Response;
  using Value = TypeCommandTag::Authorisation;
};

template <>
struct TypeCommandTraits<TypeCommand::AuthorisationRequest>
{
  using Category = TypeCommandTag::Request;
  using Value = TypeCommandTag::Authorisation;
};

template <>
struct TypeCommandTraits<TypeCommand::EchoResponse>
{
  using Category = TypeCommandTag::Response;
  using Value = TypeCommandTag::Echo;
};

template <>
struct TypeCommandTraits<TypeCommand::EchoRequest>
{
  using Category = TypeCommandTag::Request;
  using Value = TypeCommandTag::Echo;
};

template <>
struct TypeCommandTraits<TypeCommand::JoinRoomResponse>
{
  using Category = TypeCommandTag::Response;
  using Value = TypeCommandTag::JoinRoom;
};

template <>
struct TypeCommandTraits<TypeCommand::JoinRoomRequest>
{
  using Category = TypeCommandTag::Request;
  using Value = TypeCommandTag::JoinRoom;
};

template <>
struct TypeCommandTraits<TypeCommand::HistoryResponse>
{
  using Category = TypeCommandTag::Response;
  using Value = TypeCommandTag::History;
};

template <>
struct TypeCommandTraits<TypeCommand::HistoryRequest>
{
  using Category = TypeCommandTag::Request;
  using Value = TypeCommandTag::History;
};

template <>
struct TypeCommandTraits<TypeCommand::ChannelsResponse>
{
  using Category = TypeCommandTag::Response;
  using Value = TypeCommandTag::Channels;
};

template <>
struct TypeCommandTraits<TypeCommand::ChannelsRequest>
{
  using Category = TypeCommandTag::Request;
  using Value = TypeCommandTag::Channels;
};

} // namespace Traits
