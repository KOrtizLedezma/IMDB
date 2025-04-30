#include "PersistenceManager.hpp"
#include <fstream>
#include <iostream>

PersistenceManager::PersistenceManager(const std::string &filename) : filename(filename) {}

void PersistenceManager::save(const DataStore& datastore) {
  std::ofstream outfile(filename, std::ios::trunc);
  if (!outfile.is_open()) {
    Logger::log("Error: Could not open file for saving!", LogLevel::ERROR);
    return;
  }

  for (const auto& pair : datastore.getAll()) {
    outfile << pair.first << " " << pair.second << "\n";
  }

  outfile.close();
  Logger::log("Database saved to " + filename, LogLevel::INFO);
}

void PersistenceManager::load(DataStore& datastore) {
  std::ifstream infile(filename);

  if(!infile.is_open()) {
    Logger::log("No database file found, starting fresh.", LogLevel::WARNING);
    return;
  }

  std::string key, value;
  int count = 0;

  datastore.disableEviction();

  while (infile >> key >> value) {
    datastore.set(key, value);
    count++;
  }

  datastore.enableEviction();

  if (count == 0) {
    Logger::log("Database loaded but is empty.", LogLevel::WARNING);
  } else {
    Logger::log("Database loaded from " + filename + " with " + std::to_string(count) + " entries.", LogLevel::INFO);
  }

  infile.close();
}