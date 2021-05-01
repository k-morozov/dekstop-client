#pragma once

#include <string>
#include <memory>
#include <filesystem>
#include <sstream>

#include <sqlite3.h>
#include <boost/log/trivial.hpp>

#include "protocol/protocol.h"
#include "icache.h"

namespace Storage {

using namespace std::string_literals;
namespace fs = std::filesystem;

struct SqliteConfig
{
  std::string folder_path;
  std::string connection_string;
};

using sqlite3_ptr = std::unique_ptr<sqlite3, int(*)(sqlite3*)>;

class SqliteCache : ICache<std::string, msg_text_t>
{
public:
  using key_t         = typename ICache<std::string, msg_text_t>::key_t;
  using value_t       = typename ICache<std::string, msg_text_t>::value_t;
  using key_list_t    = typename ICache<std::string, msg_text_t>::key_list_t;
  using value_list_t  = typename ICache<std::string, msg_text_t>::value_list_t;

  explicit SqliteCache(SqliteConfig config)
    : config_(std::move(config))
    , db_(nullptr, &sqlite3_close)
  {
    create_folder();
    create_database();
    create_table();
  }

  bool save(const key_t& key, const value_t& value) override
  {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(4) << value.dt.date.year
       << std::setfill('0') << std::setw(2) << value.dt.date.month
       << std::setfill('0') << std::setw(2) << value.dt.date.day
       << std::setfill('0') << std::setw(2) << value.dt.time.hours
       << std::setfill('0') << std::setw(2) << value.dt.time.minutes
       << std::setfill('0') << std::setw(2) << value.dt.time.seconds;
  
    const auto query = 
      "INSERT INTO history VALUES ('"s
      + value.author + "', '"s
      + key + "', '"s
      + ss.str() + "', '"s
      + value.text + "');"s;
  
    char* raw_msg = nullptr;
  
    const auto code = sqlite3_exec(db_.get(), query.c_str(),
      nullptr, /* Callback function int(*callback)(void*, int, char**, char**) */
      nullptr, /* 1st argument to callback */
      &raw_msg
    ); 
  
    const std::unique_ptr<char[], decltype(&sqlite3_free)> msg_ptr(raw_msg, &sqlite3_free);
  
    if (code != SQLITE_OK) {
      BOOST_LOG_TRIVIAL(error) << "SQL error save: " << msg_ptr.get();
      return false;
    }

    return true;
  }

  value_list_t load(const key_t& key) const override
  {
    const auto query =
      "SELECT * FROM history WHERE channel_name='"s + key + "';"s;  
  
    sqlite3_stmt* raw_stmt = nullptr;
  
    auto code = sqlite3_prepare_v2(db_.get(), query.c_str(),
      static_cast<int>(query.size()), &raw_stmt,
      nullptr /* OUT: Pointer to unused portion of zSql */
    );
  
    const std::unique_ptr<sqlite3_stmt, int(*)(sqlite3_stmt*)> stmt_ptr(raw_stmt, &sqlite3_finalize);
  
    value_list_t history;
  
    while ((code = sqlite3_step(stmt_ptr.get())) == SQLITE_ROW) {
      value_t msg;
  
      msg.author        = static_cast<const char*>(sqlite3_column_blob(stmt_ptr.get(), 0));
      msg.channel_name  = key;
      std::string dt    = static_cast<const char*>(sqlite3_column_blob(stmt_ptr.get(), 2));
      msg.text          = static_cast<const char*>(sqlite3_column_blob(stmt_ptr.get(), 3));
  
      msg.dt.date.year    = std::stoi(dt.substr(0, 4));
      msg.dt.date.month   = std::stoi(dt.substr(4, 2));
      msg.dt.date.day     = std::stoi(dt.substr(6, 2));
      msg.dt.time.hours   = std::stoi(dt.substr(8, 2));
      msg.dt.time.minutes = std::stoi(dt.substr(10, 2));
      msg.dt.time.seconds = std::stoi(dt.substr(12, 2));
  
      history.push_back(std::move(msg));
    }
  
    if (code != SQLITE_DONE) {
      BOOST_LOG_TRIVIAL(error) << "load: while performing sql: " << sqlite3_errmsg(db_.get());
    }
  
    return history;
  }

  key_list_t keys() const override
  {
    const auto query = "SELECT DISTINCT channel_name FROM history"s;

    sqlite3_stmt* raw_stmt = nullptr;
    
    auto code = sqlite3_prepare_v2(db_.get(), query.c_str(),
      static_cast<int>(query.size()), &raw_stmt,
      nullptr /* OUT: Pointer to unused partion of zSql */
    );

    const std::unique_ptr<sqlite3_stmt, int(*)(sqlite3_stmt*)> stmt_ptr(raw_stmt, &sqlite3_finalize);

    key_list_t keys;

    while ((code = sqlite3_step(stmt_ptr.get())) == SQLITE_ROW) {
      const key_t key = static_cast<const char*>(sqlite3_column_blob(stmt_ptr.get(), 0));
      keys.push_back(std::move(key));
    }

    if (code != SQLITE_DONE) {
      BOOST_LOG_TRIVIAL(error) << "keys: while performing sql: " << sqlite3_errmsg(db_.get());
    }

    return keys;
  }

protected:
  const SqliteConfig config_;
  sqlite3_ptr db_;

private:
  const std::string create_table_query_ = 
    "CREATE TABLE IF NOT EXISTS history("s
    + "author VARCHAR["s + std::to_string(Block::LoginName) + "], "s
    + "channel_name VARCHAR["s + std::to_string(Block::LoginName) + "], "s
    + "datetime VARCHAR[14], "s
    + "message VARCHAR["s + std::to_string(Block::TextMessage) + "]);"s;

  void create_folder()
  {
    BOOST_LOG_TRIVIAL(info) << "SqliteDatabase()";
  
    if (!fs::exists(config_.folder_path)) {
      fs::create_directory(config_.folder_path);
      BOOST_LOG_TRIVIAL(info) << "Create dir for appchat cache: " << config_.folder_path;
    }
  }
  void create_database()
  {
    sqlite3* raw_db = nullptr;
  
    const auto code = sqlite3_open_v2(config_.connection_string.c_str(), &raw_db,
      SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE | SQLITE_OPEN_URI, nullptr
    );
  
    db_.reset(raw_db);
  
    if (code) {
      BOOST_LOG_TRIVIAL(fatal) << "Cannot open database: " << sqlite3_errmsg(db_.get());
    }
  }

  void create_table()
  {
    char* raw_msg = nullptr;
  
    const auto code = sqlite3_exec(db_.get(), create_table_query_.c_str(),
      nullptr, /* Callback function int(*callback)(void*, int, char**, char**) */
      nullptr, /* 1st argument to callback */
      &raw_msg
    );
  
    const std::unique_ptr<char[], void(*)(void*)> msg_ptr(raw_msg, &sqlite3_free);
  
    if (code != SQLITE_OK) {
      BOOST_LOG_TRIVIAL(error) << "SQL error create_table: " << msg_ptr.get();
    }
  }
};


} // namespace Storage
