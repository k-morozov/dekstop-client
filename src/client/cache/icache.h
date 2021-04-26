#pragma once

#include <deque>

namespace Storage {

template <class Key, class Value>
class ICache
{
public:
  using key_t         = Key;
  using value_t       = Value;
  using key_list_t    = std::deque<key_t>;
  using value_list_t  = std::deque<value_t>;

  virtual ~ICache() = default;

  virtual bool save(const key_t& key, const value_t& value) = 0;
  virtual value_list_t load(const key_t& key) const = 0;
  virtual key_list_t keys() const = 0;
};

} // namespace Storage
