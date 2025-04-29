#include "../src/datastore/DataStore.hpp"
#include <iostream>
#include <cassert>

void run_tests() {
  DataStore db;

  // Test 1: Set and Get
  db.set("name", "kenet");
  assert(db.get("name") == "kenet");

  // Test 2: Exists
  assert(db.exists("name") == true);
  assert(db.exists("age") == false);

  // Test 3: Delete
  db.del("name");
  assert(db.exists("name") == false);

  // Test 4: Get after delete (should throw)
  try {
      db.get("name");
      assert(false);
  } catch (const std::runtime_error& e) {
      assert(true);
  }

  std::cout << "All DataStore tests passed!" << std::endl;
}

int main() {
  run_tests();
  return 0;
}
