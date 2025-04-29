#pragma once

#include <string>
#include <fstream>

enum class LogLevel {
  INFO,
  WARNING,
  ERROR
};

class Logger {
public:
  static void init(const std::string& logFile = "log.txt");
  static void log(const std::string& message, LogLevel level = LogLevel::INFO);
  static void close();

private:
  static std::ofstream logfile;
  static std::string getTimestamp();
  static std::string levelToString(LogLevel level);
};
