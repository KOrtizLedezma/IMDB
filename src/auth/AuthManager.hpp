#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

enum class Role {
  USER,
  ADMIN
};

class AuthManager {
public:
  explicit AuthManager(const std::string& filename = "user.txt");

  bool authenticate(int socket_fd, const std::string& user, const std::string& password);
  bool isAuthenticated(int socket_fd) const;
  bool isAdmin(int socket_fd) const;

  void logout(int socket_fd);
  void promoteUser(const std::string& username);
  bool createUser(const std::string& username, const std::string& password, Role role);

  void shutdown();
  bool isShutdownRequested() const;

  Role getUserRole(const std::string&username) const;
  std::string getUsername(int socket_fd) const;

private:
  mutable std::mutex mutex;

  std::unordered_map<std::string, std::string> credentials;
  std::unordered_map<std::string, Role> userRoles;
  std::unordered_map<int, std::string> activeSessions;

  bool shutdownRequested = false;
  std::string userFile;
  const std::string salt = "@db#salt!";

  void ensureDefaultAdmin();
  void saveUsersToFile();
  void loadUsersFromFile();
};