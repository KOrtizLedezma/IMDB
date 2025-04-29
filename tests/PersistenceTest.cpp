#include "../src/datastore/DataStore.hpp"
#include "../src/persistence/PersistenceManager.hpp"
#include <iostream>
#include <cassert>
#include <fstream>

// Helper to delete test file
void cleanup_test_file(const std::string& filename) {
    std::remove(filename.c_str());
}

void run_persistence_tests() {
    std::cout << "🔍 Running PersistenceManager tests...\n";
    const std::string testFile = "test_database.txt";

    // === Test 1: Save and Load ===
    {
        DataStore store;
        store.set("name", "kenet");
        store.set("lang", "C++");

        PersistenceManager pm(testFile);
        pm.save(store);

        DataStore loaded;
        pm.load(loaded);

        assert(loaded.get("name") == "kenet");
        assert(loaded.get("lang") == "C++");
    }

    // === Test 2: Load from empty file ===
    {
        std::ofstream empty(testFile, std::ios::trunc);  // create empty file
        empty.close();

        DataStore loaded;
        PersistenceManager pm(testFile);
        pm.load(loaded);

        assert(!loaded.exists("name"));
    }

    // === Test 3: Overwrite old data ===
    {
        DataStore store;
        store.set("a", "1");
        PersistenceManager pm(testFile);
        pm.save(store);

        store.set("a", "99");
        store.set("b", "2");
        pm.save(store);

        DataStore loaded;
        pm.load(loaded);

        assert(loaded.get("a") == "99");
        assert(loaded.get("b") == "2");
    }

    cleanup_test_file(testFile);

    std::cout << "PersistenceManager tests passed\n";
}

int main() {
    run_persistence_tests();
    return 0;
}
