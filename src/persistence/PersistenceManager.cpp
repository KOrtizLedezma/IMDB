#include "PersistenceManager.hpp"
#include <fstream>
#include <iostream>

PersistenceManager::PersistenceManager(const std::string &filename) : filename(filename) {}

void PersistenceManager::save(const DataStore& datastore) {
  std::ofstream outfile(filename, std::ios::trunc);
  if (!outfile.is_open()) {
    std::cerr << "Error: Could not open file for saving!" << std::endl;
    return;
  }

  for (const auto& pair : datastore.getAll()) {
    outfile << pair.first << " " << pair.second << "\n";
  }

  outfile.close();
  std::cout << "Database saved to " << filename << std::endl;
}

void PersistenceManager::load(DataStore& datastore) {
  std::ifstream infile(filename);

  if(!infile.is_open()) {
    std::cerr << "Warning: No database file found, starting fresh." << std::endl;
    return;
  }

  std::string key, value;
  while (infile >> key >> value) {
    datastore.set(key, value);
  }

  infile.close();
  std::cout << "Database loaded from " << filename << std::endl;
}