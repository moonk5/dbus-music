#ifndef LOG_H
#define LOG_H

#include "spdlog/sinks/basic_file_sink.h"
#include <iostream>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <string>

class Log {
public:
  const std::string LOG_FILENAME = "dbus-music";
  Log() {
    try {
      _logger = spdlog::basic_logger_mt(LOG_FILENAME, LOG_FILENAME + ".log");
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "ERROR: Log init failed: " << ex.what() << std::endl;
    }
  }

  static void error(const std::string &msg) {
    if (_logger != nullptr) {
      _logger->error(msg);
    }
  }

private:
  static std::shared_ptr<spdlog::logger> _logger;
};

#endif /* LOG_H */
