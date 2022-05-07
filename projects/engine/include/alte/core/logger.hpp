#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <alte/export.hpp>
#include <glm/gtx/string_cast.hpp>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/spdlog.h>
#include <string>

namespace AltE::Core::Logger {

  void ALTE_API create_logger(const std::string &name);

} // namespace AltE::Core::Logger

template <> struct fmt::formatter<glm::vec3> : fmt::formatter<std::string> {
    template <typename FormatContext>
    auto format(const glm::vec3 &vector, FormatContext &ctx) {
      return fmt::formatter<std::string>::format(glm::to_string(vector), ctx);
    }
};