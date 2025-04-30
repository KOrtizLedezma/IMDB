#include "DataStore.hpp"
#include "../logger/Logger.hpp"
#include <stdexcept>

DataStore::DataStore(size_t maxMemoryBytes) : maxMemoryBytes(maxMemoryBytes), currentMemoryBytes(0), evictionEnabled(true) {}

void DataStore::disableEviction() {
  std::lock_guard<std::mutex> lock(mutex);
  evictionEnabled = false;
}

void DataStore::enableEviction() {
  std::lock_guard<std::mutex> lock(mutex);
  evictionEnabled = true;
}

void DataStore::touch(std::unordered_map<std::string, std::pair<std::string, ListIt>>::iterator it) {
  lruList.erase(it->second.second);
  lruList.push_front(it->first);
  it->second.second = lruList.begin();
}

void DataStore::evictIfNeeded(){
  while(currentMemoryBytes > maxMemoryBytes && !lruList.empty()){
    const std::string lruKey = lruList.back();
    const std::string lruVal = store[lruKey].first;

    Logger::log("Evicting key: " + lruKey, LogLevel::WARNING);

    currentMemoryBytes -= lruKey.size() + lruVal.size();
    store.erase(lruKey);
    lruList.pop_back();
  }
}

void DataStore::set(const std::string& key, const std::string& value) {
  std::lock_guard<std::mutex> lock(mutex);

  size_t entrySize = key.size() + value.size();

  auto it = store.find(key);
  if (it != store.end()) {
    currentMemoryBytes -= it->second.first.size();
    it->second.first = value;
    currentMemoryBytes += value.size();
    touch(it);
  } else {
    lruList.push_front(key);
    store[key] = { value, lruList.begin() };
    currentMemoryBytes += entrySize;
  }

  if (evictionEnabled) {
    evictIfNeeded();
  }
}

std::string DataStore::get(const std::string& key) const {
  std::lock_guard<std::mutex> lock(mutex);
  auto it = store.find(key);
  if (it == store.end()) return "NOT_FOUND";

  auto& self = *const_cast<DataStore*>(this);
  auto nonconst_it = self.store.find(key);
  self.touch(nonconst_it);

  return it->second.first;
}

void DataStore::del(const std::string& key) {
  std::lock_guard<std::mutex> lock(mutex);
  auto it = store.find(key);
  if (it != store.end()) {
    currentMemoryBytes -= key.size() + it->second.first.size();
    lruList.erase(it->second.second);
    store.erase(it);
  }
}

bool DataStore::exists(const std::string& key) const {
  std::lock_guard<std::mutex> lock(mutex);
  auto it = store.find(key);
  if (it == store.end()) return false;

  auto& self = *const_cast<DataStore*>(this);
  auto nonconst_it = self.store.find(key);
  self.touch(nonconst_it);

  return true;
}

std::unordered_map<std::string, std::string> DataStore::getAll() const {
  std::lock_guard<std::mutex> lock(mutex);
  std::unordered_map<std::string, std::string> copy;
  for (const auto& [key, val] : store) {
    copy[key] = val.first;
  }
  return copy;
}