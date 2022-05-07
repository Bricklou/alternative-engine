#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "../export.hpp"
#include <spdlog/spdlog.h>
#include <string>

namespace AltE::Core::Logger {

void ALTE_API create_logger(const std::string &name);

} // namespace AltE::Core::Logger