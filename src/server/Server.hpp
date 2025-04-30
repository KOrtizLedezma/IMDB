#pragma once

#include "../datastore/DataStore.hpp"
#include "../persistence/PersistenceManager.hpp"
#include "../worker/WorkerPool.hpp"
#include "../auth/AuthManager.hpp"
#include "../subscription/subscriptionManager.hpp"

class Server {
public:
  explicit Server(int port, DataStore& datastore, PersistenceManager& persistence);
  void publishEvent(const std::string& eventType, int client_socket, const std::string& key, const std::string& extra = "");
  void start();

private:
  int port;
  DataStore& datastore;
  PersistenceManager& persistence;
  WorkerPool pool;
  AuthManager auth;
  SubscriptionManager subscriptions;

  void handleClient(int client_socket);
};