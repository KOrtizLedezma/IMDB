#include "Server.hpp"
#include "../datastore/DataStore.hpp"
#include "../command/CommandParser.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

Server::Server(int port, DataStore& datastore, PersistenceManager& persistence) : port(port), datastore(datastore), persistence(persistence), pool(4) {}

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
  char buffer[1024];
  while (true) {
    memset(buffer, 0, sizeof(buffer));
    int bytes_read = read(client_socket, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
      Logger::log("Client disconnected", LogLevel::ERROR);
      close(client_socket);
      break;
    }

    std::string input(buffer);
    input.erase(input.find_last_not_of(" \n\r\f\v") + 1);

    Logger::log("Received: " + input, LogLevel::INFO);

    auto command = CommandParser::parse(input);
    std::string response;

    if (command.type == CommandType::SET) {
      datastore.set(command.key, command.value);
      persistence.save(datastore);
      response = "OK\n";
    }
    else if (command.type == CommandType::GET) {
      response = datastore.get(command.key);
    }
    else if (command.type == CommandType::DEL) {
      datastore.del(command.key);
      persistence.save(datastore);
      response = "OK\n";
    }
    else if (command.type == CommandType::EXISTS) {
      response = datastore.exists(command.key) ? "1\n" : "0\n";
    }
    else if (command.type == CommandType::EXIT) {
      Logger::log("Server exiting", LogLevel::INFO);
      close(client_socket);
      break;
    }
    else {
      Logger::log("Unknown command", LogLevel::ERROR);
    }

    write(client_socket, response.c_str(), response.length());
  }
}