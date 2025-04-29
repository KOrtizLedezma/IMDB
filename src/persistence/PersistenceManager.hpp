#pragma once

#include "../datastore/DataStore.hpp"
#include <string>

class PersistenceManager {

public:
  PersistenceManager(const std::string& filename);

  void save(const DataStore& datastore);
  void load(DataStore& datastore);

private:
  std::string filename;

};