#include "Utils.hpp"

std::vector<std::string> Utils::split(const std::string& s, char delim) {
  std::vector<std::string> tokens;
  std::istringstream iss(s);
  std::string token;

  while (std::getline(iss, token, delim)) {
    if (!token.empty()) {
      tokens.push_back(token);
    }
  }
  return tokens;
}