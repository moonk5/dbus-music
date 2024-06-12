#ifndef LOG_H
#define LOG_H

#include "spdlog/sinks/basic_file_sink.h"
#include <iostream>
#include <memory>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <string>

class Log {
public:
  Log() {}

  Log(const std::string &filename) : log_filename(filename) {
    try {
      logger = spdlog::basic_logger_mt(log_filename, log_filename + ".log");

      spdlog::set_level(spdlog::level::debug);
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "ERROR: Log init failed: " << ex.what() << std::endl;
    }
  }

  void debug(const std::string &msg) {

    if (logger != nullptr) {
      logger->debug(msg);
      std::cout << "logger is not NULL" << std::endl;

    } else {
      std::cout << "logger is NULL" << std::endl;
    }
  }

  void error(const std::string &msg) {
    if (logger != nullptr) {
      logger->error(msg);
    }
  }

private:
  std::string log_filename;
  std::shared_ptr<spdlog::logger> logger;
};

#endif /* LOG_H */
