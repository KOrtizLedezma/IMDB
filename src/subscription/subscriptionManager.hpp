#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

class SubscriptionManager {

public:
  void subscribe(int client_fd, const std::string& eventType);
  void unsubscribe(int client_fd);
  void notify(const std::string& eventType, const std::string& message);

private:
  std::unordered_map<std::string, std::unordered_set<int>> subscribers;
  std::mutex mutex;

};