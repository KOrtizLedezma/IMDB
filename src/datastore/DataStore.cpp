// src/datastore/DataStore.cpp
#include "DataStore.hpp"
#include <stdexcept>

DataStore::DataStore() {
}

void DataStore::set(const std::string& key, const std::string& value) {
  store[key] = value;
}

std::string DataStore::get(const std::string& key) const{
  auto it = store.find(key);
  if (it != store.end()) {
    return it->second;
  }
  return "NOT_FOUND";
}

void DataStore::del(const std::string& key) {
  store.erase(key);
}

bool DataStore::exists(const std::string& key) const {
  return store.find(key) != store.end();
}