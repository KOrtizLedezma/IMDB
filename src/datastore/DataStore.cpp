#include "DataStore.hpp"
#include <stdexcept>

DataStore::DataStore() {
}

void DataStore::set(const std::string& key, const std::string& value) {
  std::unique_lock lock(mutex);
  store[key] = value;
}

std::string DataStore::get(const std::string& key) const{
  std::shared_lock lock(mutex);
  auto it = store.find(key);
  if (it != store.end()) {
    return it->second;
  }
  return "NOT_FOUND";
}

void DataStore::del(const std::string& key) {
  std::unique_lock lock(mutex);
  store.erase(key);
}

bool DataStore::exists(const std::string& key) const {
  std::shared_lock lock(mutex);
  return store.find(key) != store.end();
}

std::vector<std::pair<std::string, std::string>> DataStore::getAll() const {
  std::shared_lock lock(mutex);
  std::vector<std::pair<std::string, std::string>> data;
  for (const auto& pair : store) {
    data.push_back(pair);
  }
  return data;
}