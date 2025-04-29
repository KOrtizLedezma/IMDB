#pragma once

#include "../datastore/DataStore.hpp"
#include "../persistence/PersistenceManager.hpp"
#include "../worker/WorkerPool.hpp"

class Server {
public:
  explicit Server(int port, DataStore& datastore, PersistenceManager& persistenceManager);
  void start();

private:
  int port;
  DataStore& datastore;
  PersistenceManager& persistence;
  WorkerPool pool;

  void handleClient(int client_socket);
};