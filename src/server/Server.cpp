#include "Server.hpp"
#include "../datastore/DataStore.hpp"
#include "../command/CommandParser.hpp"
#include "../utils/Utils.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

Server::Server(int port, DataStore& datastore, PersistenceManager& persistence) : port(port), datastore(datastore), persistence(persistence), pool(4), auth("user.txt") {}

void Server::start() {
  int server_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  // Create a socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("Socket failed");
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt failed");
    exit(EXIT_FAILURE);
  }

  // Bind
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
    perror("Bind failed");
    exit(EXIT_FAILURE);
  }

  // Listen
  if (listen(server_fd, 10) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  std::cout << "Server listening on port " << port << std::endl;

  // Accept
  while (true) {
    int client_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
    if (client_socket < 0) {
      Logger::log("Server Accept failed", LogLevel::ERROR);
      continue;
    }
    Logger::log("New client connected", LogLevel::INFO);

    pool.enqueue([this, client_socket]() {
        handleClient(client_socket);
    });
  }
}

void Server::handleClient(int client_socket) {
  char buffer[1024] = {0};
  const std::string salt = "@db#salt!";

  std::string welcome = "Welcome to InMemoryDB!\nPlease authenticate using: AUTH <username> <password>\n";
  write(client_socket, welcome.c_str(), welcome.length());

  // Auth loop
  while (true) {
    memset(buffer, 0, sizeof(buffer));

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(client_socket, &fds);

    struct timeval timeout;
    timeout.tv_sec = 300;
    timeout.tv_usec = 0;

    int activity = select(client_socket + 1, &fds, nullptr, nullptr, &timeout);

    if (activity == 0) {
      Logger::log("Client disconnected", LogLevel::INFO);
      auth.logout(client_socket);
      close(client_socket);
      return;
    }

    int bytes_read = read(client_socket, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
      Logger::log("Client disconnected (before auth)", LogLevel::INFO);
      close(client_socket);
      return;
    }

    std::string input(buffer);
    input.erase(input.find_last_not_of(" \n\r\t") + 1);

    auto tokens = Utils::split(input);

    if (tokens.size() == 3 && tokens[0] == "AUTH") {
      if (auth.authenticate(client_socket, tokens[1], tokens[2])) {
        Logger::log("Authenticated: " + tokens[1],  LogLevel::INFO);
        std::string msg = "Authenticated!\n";
        write(client_socket, msg.c_str(), msg.size());
        break;
      }
      else {
        std::string msg = "Invalid credentials.\n";
        write(client_socket, msg.c_str(), msg.size());
      }
    }
    else {
      std::string msg = "You must authenticate first.\n";
      write(client_socket, msg.c_str(), msg.size());
    }
  }

  // Main Command Loop
  while (true) {
    memset(buffer, 0, sizeof(buffer));

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(client_socket, &fds);

    struct timeval timeout;
    timeout.tv_sec = 300;
    timeout.tv_usec = 0;

    int activity = select(client_socket + 1, &fds, nullptr, nullptr, &timeout);

    if (activity == 0) {
      Logger::log("Client disconnected", LogLevel::INFO);
      auth.logout(client_socket);
      close(client_socket);
      return;
    }

    int bytes_read = read(client_socket, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
      Logger::log("Client disconnected: " + auth.getUsername(client_socket), LogLevel::INFO);
      auth.logout(client_socket);
      close(client_socket);
      return;
    }

    std::string input(buffer);
    input.erase(input.find_last_not_of(" \n\r\t") + 1);
    Logger::log("[" + auth.getUsername(client_socket) + "] " + input, LogLevel::INFO);

    auto command = CommandParser::parse(input);
    auto tokens = Utils::split(input);
    std::string response;

    if (!auth.isAuthenticated(client_socket)) {
      response = "You are not authenticated.\n";
    }
    else if (command.type == CommandType::SET) {
      datastore.set(command.key, command.value);
      persistence.save(datastore);
      response = "OK\n";
    }
    else if (command.type == CommandType::GET) {
      response = datastore.get(command.key) + "\n";
    }
    else if (command.type == CommandType::DEL) {
      datastore.del(command.key);
      persistence.save(datastore);
      response = "OK\n";
      publishEvent("DEL", client_socket, command.key);
    }
    else if (command.type == CommandType::EXISTS) {
      response = datastore.exists(command.key) ? "1\n" : "0\n";
    }
    else if (command.type == CommandType::GETALL) {
      if (!auth.isAdmin(client_socket)) {
        response = "Admins only.\n";
      }
      else {
        for (const auto& [k, v] : datastore.getAll()) {
          response += k + " = " + v + "\n";
        }
      }
    }
    else if (command.type == CommandType::DELALL) {
      if (!auth.isAdmin(client_socket)) {
        response = "Admins only.\n";
      }
      else {
        for (const auto& [k, _] : datastore.getAll()) {
          datastore.del(k);
          publishEvent("DELALL", client_socket, k);
        }
        persistence.save(datastore);
        response = "All keys deleted.\n";
      }
    }
    else if (command.type == CommandType::PROMOTE) {
      if (!auth.isAdmin(client_socket)) {
        response = "Admins only.\n";
      }
      else {
        auth.promoteUser(command.key);
        response = "User promoted to admin.\n";
      }
    }
    else if (command.type == CommandType::SHUTDOWN) {
      if (!auth.isAdmin(client_socket)) {
        response = "Admins only.\n";
      }
      else {
        Logger::log("Shutdown requested by " + auth.getUsername(client_socket));
        response = "Server will shut down.\n";
        write(client_socket, response.c_str(), response.length());
        auth.logout(client_socket);
        close(client_socket);
        auth.shutdown();
        exit(0);
        return;
      }
    }
    else if (tokens.size() == 4 && tokens[0] == "CREATEUSER") {
      if (!auth.isAdmin(client_socket)) {
        response = "Admins only.\n";
      }
      else {
        Role role = (tokens[3] == "ADMIN") ? Role::ADMIN : Role::USER;
        if (auth.createUser(tokens[1], tokens[2] , role)) {
          response = "User created.\n";
          Logger::log("User created successfully.", LogLevel::INFO);
        }
        else {
          response = "User creation failed.\n";
        }
      }
    }
    else if (command.type == CommandType::EXIT) {
      response = "Goodbye!\n";
      Logger::log("[" + auth.getUsername(client_socket) + "] exited.");
      auth.logout(client_socket);
      write(client_socket, response.c_str(), response.length());
      close(client_socket);
      return;
    }
    else if (tokens.size() == 2 && tokens[0] == "SUBSCRIBE") {
      if (tokens[1] != "DEL" && tokens[1] != "DELALL") {
        response = "Only DEL and DELALL subscriptions are allowed.\n";
      } else {
        subscriptions.subscribe(client_socket, tokens[1]);
        response = "Subscribed to " + tokens[1] + "\n";
      }
    }
    else if (tokens.size() == 1 && tokens[0] == "UNSUBSCRIBE") {
      subscriptions.unsubscribe(client_socket);
      response = "Unsubscribed from all events.\n";
    }
    else {
      response = "ERROR: Invalid command\n";
    }

    write(client_socket, response.c_str(), response.length());
  }
}

void Server::publishEvent(const std::string& eventType, int client_socket, const std::string& key, const std::string& extra) {
  std::string user = auth.getUsername(client_socket);
  std::string message = "[EVENT:" + eventType + "] " + user + " deleted " + key;
  if (!extra.empty()) message += " (" + extra + ")";
  message += "\n";

  subscriptions.notify(eventType, message);
}
