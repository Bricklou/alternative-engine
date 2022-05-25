#include "OpenALErrorCheck.hpp"
#include "AL/al.h"
#include <spdlog/spdlog.h>
#include <utility>

namespace {
  namespace ALCheckImpl {
    thread_local ALenum lastError(AL_NO_ERROR);
  }
} // namespace

namespace AltE {
  void al_check_error(const std::filesystem::path &file, uint32_t line,
                      const char *expression) {
    // Get the last error
    ALenum error_code = alGetError();

    if (error_code != AL_NO_ERROR) {
      ALCheckImpl::lastError = error_code;

      std::string error = "Unknow error";
      std::string description = "No description";

      // Decode the error code
      switch (error_code) {
        case AL_INVALID_NAME:
          {
            error = "AL_INVALID_NAME";
            description = "A bad name (ID) has been specified.";
            break;
          }

        case AL_INVALID_ENUM:
          {
            error = "AL_INVALID_ENUM";
            description = "An unacceptable value has been specified for an "
                          "enumerated argument.";
            break;
          }

        case AL_INVALID_VALUE:
          {
            error = "AL_INVALID_VALUE";
            description = "A numeric argument is out of range.";
            break;
          }

        case AL_INVALID_OPERATION:
          {
            error = "AL_INVALID_OPERATION";
            description =
                "The specified operation is not allowed in the current state.";
            break;
          }

        case AL_OUT_OF_MEMORY:
          {
            error = "AL_OUT_OF_MEMORY";
            description =
                "There is not enough memory left to execute the command.";
            break;
          }
      }

      // Log the error
      SPDLOG_ERROR("An internal OpenAL call failed in {}:{}."
                   "\nExpression:\n\t{}"
                   "\nError description:\n\t{}\n"
                   "\t{}\n",
                   file.filename().string(), line, expression, error,
                   description);
    }
  }

  ALenum al_get_last_error_impl() {
    return std::exchange(ALCheckImpl::lastError, AL_NO_ERROR);
  }
} // namespace AltE