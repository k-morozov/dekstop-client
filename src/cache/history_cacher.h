#pragma once

#include "sqlitecache.h"
#include "client/client.h"

#include <QObject>
#include <memory>

namespace Storage {

class HistoryCacher : public QObject
{
  Q_OBJECT

public:
  using cache_t   = SqliteCache;
  using client_t  = Client;

  explicit HistoryCacher(cache_t&& cache, QObject* parent = nullptr);

  virtual ~HistoryCacher();

public:
  void set_client(std::shared_ptr<client_t> client);

protected slots:
  void on_authorized(goodok::command::StatusCode code);

  void on_joined(const std::string& channel_name);
  
  void on_message_received(const goodok::command::msg_text_t& message);

  void on_history_received(const std::string& channel_name, const std::vector<goodok::command::msg_text_t>& hist);

  void on_channels_received(const std::vector<std::string>& channels);

protected:
  cache_t cache_;
  std::shared_ptr<client_t> client_;
};


} // namespace Storage
