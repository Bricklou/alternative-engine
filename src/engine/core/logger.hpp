#pragma once

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace AltE::Logger {

  void createLogger(const std::string &name);

} // namespace AltE::Logger