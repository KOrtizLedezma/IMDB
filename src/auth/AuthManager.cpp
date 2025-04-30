#include "AuthManager.hpp"
#include "../crypto/SHA256.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

AuthManager::AuthManager(const std::string& filename) :userFile(filename) {
  loadUsersFromFile();
  ensureDefaultAdmin();
}

void AuthManager::loadUsersFromFile() {
  std::ifstream file(userFile);
  if (!file.is_open()) return;

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string username, password, roleStr;

    if (std::getline(iss, username, ':') && std::getline(iss, password, ':'), std::getline(iss, roleStr)) {
      credentials[username] = password;
      userRoles[username] = (roleStr == "ADMIN") ? Role::ADMIN : Role::USER;
    }
  }
}

void AuthManager::saveUsersToFile() {
  std::ofstream file(userFile, std::ios::trunc);
  if (!file.is_open()) return;

  for (const auto& [username, hashedPassword] : credentials) {
    std::string roleStr = (userRoles[username] == Role::ADMIN) ? "ADMIN" : "USER";
    file << username << ":" << hashedPassword << ":" << roleStr << "\n";
  }
}

void AuthManager::ensureDefaultAdmin() {
  if (!credentials.count("admin")) {
    credentials["admin"] = sha256("1234" + salt);
    userRoles["admin"] = Role::ADMIN;
    saveUsersToFile();
  }
}

bool AuthManager::createUser(const std::string& username, const std::string& password, Role role) {
  std::lock_guard<std::mutex> lock(mutex);
  if (credentials.count(username)) return false;

  credentials[username] = sha256(password + salt);
  userRoles[username] = role;
  saveUsersToFile();
  return true;
}

bool AuthManager::authenticate(int socket_fd, const std::string& username, const std::string& password) {
  std::lock_guard<std::mutex> lock(mutex);
  auto it = credentials.find(username);
  if (it != credentials.end() && it->second == sha256(password + salt)) {
    activeSessions[socket_fd] = username;
    return true;
  }
  return false;
}

bool AuthManager::isAuthenticated(int socket_fd) const{
  std::lock_guard<std::mutex> lock(mutex);
  return activeSessions.count(socket_fd);
}

bool AuthManager::isAdmin(int socket_fd) const{
  std::lock_guard<std::mutex> lock(mutex);
  auto it = activeSessions.find(socket_fd);
  if (it == activeSessions.end()) return false;

  auto roleIt = userRoles.find(it->second);
  return roleIt != userRoles.end() && roleIt->second == Role::ADMIN;
}

Role AuthManager::getUserRole(const std::string& username) const {
  std::lock_guard<std::mutex> lock(mutex);
  auto it = userRoles.find(username);
  return (it != userRoles.end()) ? it->second : Role::USER;
}

std::string AuthManager::getUsername(int socket_fd) const{
  std::lock_guard<std::mutex> lock(mutex);
  auto it = activeSessions.find(socket_fd);
  return (it != activeSessions.end()) ? it->second : "";
}

void AuthManager::logout(int socket_fd) {
  std::lock_guard<std::mutex> lock(mutex);
  activeSessions.erase(socket_fd);
}

void AuthManager::promoteUser(const std::string& username) {
  if (username.empty()) {
    return;
  }
  std::lock_guard<std::mutex> lock(mutex);
  if (credentials.count(username)) {
    userRoles[username] = Role::ADMIN;
  }
}

void AuthManager::shutdown() {
  std::lock_guard<std::mutex> lock(mutex);
  shutdownRequested = true;
}

bool AuthManager::isShutdownRequested() const{
  std::lock_guard<std::mutex> lock(mutex);
  return shutdownRequested;
}

