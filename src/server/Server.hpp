#pragma once

#include "../datastore/DataStore.hpp"
#include "../persistence/PersistenceManager.hpp"
#include "../worker/WorkerPool.hpp"
#include "../auth/AuthManager.hpp"

class Server {
public:
  explicit Server(int port, DataStore& datastore, PersistenceManager& persistence);
  void start();

private:
  int port;
  DataStore& datastore;
  PersistenceManager& persistence;
  WorkerPool pool;
  AuthManager auth;

  void handleClient(int client_socket);
};