#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <shared_mutex>
#include <mutex>

class DataStore {
public:
  // Constructor
  DataStore();

  // Basic operations
  void set(const std::string& key, const std::string& value);
  std::string get(const std::string& key) const;
  void del(const std::string& key);
  bool exists(const std::string& key) const;
  std::vector<std::pair<std::string, std::string>> getAll() const;

private:
  std::unordered_map<std::string, std::string> store;
  mutable std::shared_mutex mutex;
};