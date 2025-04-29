#include "server/Server.hpp"
#include "persistence/PersistenceManager.hpp"
#include "datastore/DataStore.hpp"
#include "logger/Logger.hpp"

int main() {
  DataStore datastore;
  PersistenceManager persistence("database.txt");
  Logger::init("log.txt");
  persistence.load(datastore);

  Server server(12345, datastore, persistence);
  server.start();

  persistence.save(datastore);
  Logger::close();
  return 0;
}
