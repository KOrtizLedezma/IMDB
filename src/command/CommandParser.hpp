#pragma once

#include <string>

enum class CommandType {
  SET,
  GET,
  DEL,
  EXISTS,
  EXIT,
  DELALL,
  GETALL,
  SHUTDOWN,
  PROMOTE,
  INVALID // Wrong commands
};

struct Command {
  CommandType type;
  std::string key;
  std::string value;
};

class CommandParser {
public:
  static Command parse(const std::string& input);
};