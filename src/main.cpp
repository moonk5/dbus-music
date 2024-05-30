#include "spdlog/spdlog.h"
#include <cstdint>
#include <dbus/dbus.h>
#include <iostream>
#include <string>

#include "log.h"

int main(void) {
  Log::error("TEST");

  return 0;
}
