#include "subscriptionManager.hpp"
#include <unistd.h>
#include <iostream>

void SubscriptionManager::subscribe(int client_fd, const std::string& eventType){
    std::lock_guard<std::mutex> lock(mutex);
    subscribers[eventType].insert(client_fd);
}

void SubscriptionManager::unsubscribe(int client_fd) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& [event, subs] : subscribers) {
      subs.erase(client_fd);
    }
}

void SubscriptionManager::notify(const std::string& eventType, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = subscribers.find(eventType);
    if (it == subscribers.end()) return;

    std::unordered_set<int> toRemove;

    for (int client_fd : it->second) {
      ssize_t bytes = write(client_fd, message.c_str(), message.size());
      if (bytes <= 0) {
        toRemove.insert(client_fd);
      }
    }

    // Clean Up disconnected subscribers
    for (int fd : toRemove) {
      it -> second.erase(fd);
    }
}