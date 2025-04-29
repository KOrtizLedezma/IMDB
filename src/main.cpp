#include "server/Server.hpp"
#include "persistence/PersistenceManager.hpp"
#include "datastore/DataStore.hpp"

int main() {
    DataStore datastore;
    PersistenceManager persistence("database.txt");

    persistence.load(datastore);

    Server server(12345, datastore, persistence);
    server.start();

    persistence.save(datastore);

    return 0;
}
