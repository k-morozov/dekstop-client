#include "history_cacher.h"

Q_DECLARE_METATYPE(std::vector<std::string>)
Q_DECLARE_METATYPE(std::vector<goodok::command::msg_text_t>)

namespace Storage {

HistoryCacher::HistoryCacher(cache_t&& cache, QObject* parent)
  : QObject(parent), cache_(std::forward<cache_t>(cache)), client_(nullptr)
{
  qRegisterMetaType<std::vector<std::string>>("std::vector<std::string>");
  qRegisterMetaType<std::vector<goodok::command::msg_text_t>>("std::vector<msg_text_t>");
}


HistoryCacher::~HistoryCacher() = default;


void HistoryCacher::set_client(std::shared_ptr<client_t> client)
{
  if (client_) {
    // disconnect from all signals from the old client
    client_->disconnect(this);
  }

  client_ = client;

  if (client_) {
    connect(client_.get(), &client_t::send_input_code, this, &HistoryCacher::on_authorized);
    connect(client_.get(), &client_t::joined, this, &HistoryCacher::on_joined);
    connect(client_.get(), &client_t::send_text, this, &HistoryCacher::on_message_received);
    connect(client_.get(), &client_t::history_received, this, &HistoryCacher::on_history_received);
    connect(client_.get(), &client_t::channels_received, this, &HistoryCacher::on_channels_received);
  }
}

void HistoryCacher::on_authorized(goodok::command::StatusCode code)
{
  if (code != goodok::command::StatusCode::AutorOK) {
    return;
  }

  // load cached history
  const auto channels = cache_.keys();
  
  for (const auto& channel : channels) {
    const auto messages = cache_.load(channel);
    const auto ptr = client_->get_ptr_history(channel);
    ptr->insert(ptr->end(), messages.begin(), messages.end());
  }
  
  // get not cached history
  client_->send_channels_request(); 
}


void HistoryCacher::on_joined(const std::string& channel_name)
{
  const auto ptr = client_->get_ptr_history(channel_name);

  const auto it = std::max_element(ptr->begin(), ptr->end(),
    [](const auto& lhs, const auto& rhs) {
      return lhs.dt < rhs.dt;
    }
  );

  const auto since = it == ptr->end() ? goodok::DateTime{} : it->dt;

  client_->send_history_request(channel_name, since);
}


void HistoryCacher::on_message_received(const goodok::command::msg_text_t& message)
{
  cache_.save(message.channel_name, message);
}


void HistoryCacher::on_history_received(const std::string& channel, const std::vector<goodok::command::msg_text_t>& hist)
{
  for (const auto& message : hist) {
    cache_.save(channel, message);
  }
}  

void HistoryCacher::on_channels_received(const std::vector<std::string>& channels)
{
  for (const auto& channel : channels) {
    const auto messages = cache_.load(channel);

      goodok::DateTime since;

    if (!messages.empty()) {
      since = messages.back().dt;
    }

    client_->send_history_request(channel, since);
  }
}

} // namespace Storage
