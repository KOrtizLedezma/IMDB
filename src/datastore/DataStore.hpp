#pragma once

#include <list>
#include <string>
#include <unordered_map>
#include <mutex>

class DataStore {
public:
  // Constructor
  explicit DataStore(size_t maxMemoryBytes = 10 * 1024 * 1024);

  // Basic operations
  void set(const std::string& key, const std::string& value);
  std::string get(const std::string& key) const;
  void del(const std::string& key);
  bool exists(const std::string& key) const;

  std::unordered_map<std::string, std::string> getAll() const;

  void disableEviction();
  void enableEviction();

private:
  size_t maxMemoryBytes;
  size_t currentMemoryBytes;
  bool evictionEnabled;

  mutable std::mutex mutex;

  using ListIt = std::list<std::string>::iterator;
  std::list<std::string> lruList;
  std::unordered_map<std::string, std::pair<std::string, ListIt>> store;

  void touch(std::unordered_map<std::string, std::pair<std::string, ListIt>>::iterator it);
  void evictIfNeeded();
};