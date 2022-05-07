#include "spdlog/logger.h"
#include "spdlog/common.h"
#include <alte/core/logger.hpp>

#include <memory>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace AltE::Core::Logger {

void create_logger(const std::string &name) {
  // Create console logger sink
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  // Create a new logger
  auto logger = std::make_shared<spdlog::logger>(name, console_sink);

  // Set the default spdlog logger to the new logger
  spdlog::set_default_logger(logger);

  // Set the default log level
  logger->set_level(spdlog::level::trace);
  spdlog::set_level(spdlog::level::trace);

  // Set the pattern for the logger (like the default one but with more colors)
  spdlog::default_logger()->set_pattern(
      "\u001b[90m[%Y-%m-%d %T.%e] [thread %t]\u001b[39m "
      "[\u001b[33m%n\u001b[39m] [%^%=9l%$] %v");
}

} // namespace AltE::Core::Logger