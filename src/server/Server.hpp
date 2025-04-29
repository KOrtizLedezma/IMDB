#pragma once

#include "../datastore/DataStore.hpp"
#include "../persistence/PersistenceManager.hpp"

class Server {
public:
  explicit Server(int port, DataStore& datastore, PersistenceManager& persistenceManager);
  void start();

private:
  int port;
  DataStore& datastore;
  PersistenceManager& persistence;
};