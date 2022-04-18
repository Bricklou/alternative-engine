#include "logger.hpp"

namespace AltE::Core::Logger {
  void createLogger(const std::string &name) {
    // Create console logger sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    // Create a new logger
    auto logger = std::make_shared<spdlog::logger>(name, console_sink);

    // Set default spdlog logger to a new one
    spdlog::set_default_logger(logger);

    // Set the default log level
    logger->set_level(spdlog::level::trace);

    // Set the pattern for the logger (like the default one but with more
    // colors)
    spdlog::default_logger()->set_pattern(
        "\033[90m[%Y-%m-%d %T.%e] [thread %t]\033[m [\033[33m%n\033[m] "
        "[%^%=9l%$] %v");
  }
} // namespace AltE::Core::Logger