
#include "spdlog/spdlog.h"
#include <alte/core/logger.hpp>
#include <stdio.h>

int main(int argc, char *argv[]) {
  AltE::Core::Logger::create_logger("alternative-engine");

  spdlog::debug("Hello world!");
}