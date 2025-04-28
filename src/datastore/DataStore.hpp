// src/datastore/DataStore.hPP
#pragma once

#include <string>
#include <unordered_map>

class DataStore {
public:
  // Constructor
  DataStore();

  // Basic operations
  void set(const std::string& key, const std::string& value);
  std::string get(const std::string& key) const;
  void del(const std::string& key);
  bool exists(const std::string& key) const;

private:
  std::unordered_map<std::string, std::string> store;
};