#include "CommandParser.hpp"

#include <sstream>
#include <vector>

Command CommandParser::parse(const std::string& input) {
  std::istringstream iss(input);
  std::vector<std::string> tokens;
  std::string token;

  while (iss >> token) {
    tokens.push_back(token);
  }

  if (tokens.empty()) {
    return {CommandType::INVALID, "", ""};
  }

  std::string command = tokens[0];

  if (command == "SET" && tokens.size() >= 3) {
    return {CommandType::SET, tokens[1], tokens[2]};
  }
  else if (command == "GET" && tokens.size() == 2) {
    return {CommandType::GET, tokens[1], ""};
  }
  else if (command == "DEL" && tokens.size() == 2) {
    return {CommandType::DEL, tokens[1], ""};
  }
  else if (command == "EXISTS" && tokens.size() == 2) {
    return {CommandType::EXISTS, tokens[1], ""};
  }
  else if (command == "EXIT" && tokens.size() == 1) {
    return {CommandType::EXIT, "", ""};
  }
  else if (command == "DELALL" && tokens.size() == 1) {
    return {CommandType::DELALL, "", ""};
  }
  else if (command == "GETALL" && tokens.size() == 1) {
    return {CommandType::GETALL, "", ""};
  }
  else if (command == "SHUTDOWN" && tokens.size() == 1) {
    return {CommandType::SHUTDOWN, "", ""};
  }
  else if (command == "PROMOTE" && tokens.size() == 2) {
    return {CommandType::PROMOTE, tokens[1], ""};
  }
  return {CommandType::INVALID, "", ""};
}