#include "Logger.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <filesystem>

std::ofstream Logger::logfile;

void Logger::init(const std::string& logFile) {
  namespace fs = std::filesystem;

  if (fs::exists(logFile)) {
    try {
      fs::rename(logFile, "log-old.txt");
    } catch (const std::filesystem::filesystem_error& e) {
      std::cerr << "[ERROR] Failed to rotate logs: " << e.what() << std::endl;
    }
  }

  logfile.open(logFile, std::ios::app);
  if (!logfile.is_open()) {
    std::cerr << "[ERROR] Failed to open log file: " << logFile << std::endl;
  }
}

void Logger::log(const std::string& message, LogLevel level) {
  std::string timestamp = getTimestamp();
  std::string levelStr = levelToString(level);
  std::string fullMessage = "[" + timestamp + "] " + levelStr + " " + message;

  std::cout << fullMessage << std::endl;

  if (logfile.is_open()) {
    logfile << fullMessage << std::endl;
  }
}

void Logger::close() {
  if (logfile.is_open()) {
    logfile.close();
  }
}

std::string Logger::getTimestamp() {
  std::time_t now = std::time(nullptr);
  std::tm* tm_now = std::localtime(&now);
  std::ostringstream oss;
  oss << std::put_time(tm_now, "%Y-%m-%d %H:%M:%S");
  return oss.str();
}

std::string Logger::levelToString(LogLevel level) {
  switch (level) {
    case LogLevel::INFO: return "[INFO]";
    case LogLevel::WARNING: return "[WARNING]";
    case LogLevel::ERROR: return "[ERROR]";
    default: return "[UNKNOWN]";
  }
}
