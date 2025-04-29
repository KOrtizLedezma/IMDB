#include "../src/datastore/DataStore.hpp"
#include <thread>
#include <vector>
#include <cassert>
#include <iostream>

void writer_thread(DataStore& store, int id) {
  for (int i = 0; i < 100; i++) {
    store.set("key" + std::to_string(id) + "_" + std::to_string(i), std::to_string(i));
  }
}

void reader_thread(DataStore& store, int id) {
  for (int i = 0 ; i < 100; i++) {
    std::string key = "key" + std::to_string(id % 2) + "_" + std::to_string(i);
    std::string value = store.get(key);
    try {
      if (value != "NOT_FOUND") {
        int val = std::stoi(value);
        assert(val == i);
       }
    }
    catch (const std::exception& e) {
      std::cerr << "[ERROR] Exception in reader_thread: " << e.what() << "\n";
    }
  }
}

int main() {
  DataStore store;

  std::vector<std::thread> threads;

  for (int i = 0; i < 2; i++) {
    threads.emplace_back(writer_thread, std::ref(store), i);
  }

  for (int i = 0; i < 4; i++) {
    threads.emplace_back(reader_thread, std::ref(store), i);
  }

  for (auto& thread : threads) {
    thread.join();
  }

  std::cout << "Threaded DataStore test passed\n" << std::endl;
  return 0;
}