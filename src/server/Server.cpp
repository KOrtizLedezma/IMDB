#include "Server.hpp"
#include "../datastore/DataStore.hpp"
#include "../command/CommandParser.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

Server::Server(int port) : port(port) {}

void Server::start() {
  int server_fd, client_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  // Create a socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("Socket failed");
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
  if (listen(server_fd, 3) < 0) {
    perror("Listen failed");
    exit(EXIT_FAILURE);
  }

  std::cout << "Server listening on port " << port << std::endl;

  // Accept
  client_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
  if (client_socket < 0) {
    perror("Accept failed");
    exit(EXIT_FAILURE);
  }

  std::cout << "Client connected" << std::endl;

  // Process client commands
  char buffer[1024] = {0};
  DataStore datastore;

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    int bytes_read = read(client_socket, buffer, sizeof(buffer));
    if (bytes_read <= 0) {
      std::cout << "Client disconnected" << std::endl;
      break;
    }

    std::string input(buffer);
    input.erase(input.find_last_of(" \n\r\t" [ 1]));

    auto command = CommandParser::parse(input);

    std::string response;

    if (command.type == CommandType::SET) {
      datastore.set(command.key, command.value);
      response = "OK\n";
    } else if (command.type == CommandType::GET) {
      std::string value = datastore.get(command.key);
      response = value + "\n";
    } else if (command.type == CommandType::DEL) {
      datastore.del(command.key);
      response = "OK\n";
    } else if (command.type == CommandType::EXISTS) {
      response = datastore.exists(command.key) ? "1\n" : "0\n";
    } else {
      response = "ERROR: Invalid command\n";
    }

    write(client_socket, response.c_str(), response.length());
  }

  close(client_socket);
  close(server_fd);
}