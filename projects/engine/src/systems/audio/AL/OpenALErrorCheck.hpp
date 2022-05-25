#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <alte/export.hpp>
#include <filesystem>
#include <stdint.h>

namespace AltE {

#ifndef NDEBUG
  #define al_check(expr)                                                       \
    {                                                                          \
      expr;                                                                    \
      AltE::al_check_error(__FILE__, __LINE__, #expr);                         \
    }

  #define al_get_last_error al_get_last_error_impl
#else

  // Else, we don't add any overhead
  #define al_check(expr) (expr)
  #define al_get_last_error alGetError

#endif

  /**
   * @brief Check the last OpenAL error
   *
   * @param file Source file where the call is located
   * @param line Line number where the call is located
   * @param expression The evaluated expression as a string
   */
  void ALTE_API al_check_error(const std::filesystem::path &file, uint32_t line,
                               const char *expression);

  /**
   * @brief Get the last OpenAL error on this thread
   *
   * @return The last OpenAL error on this thread
   */
  ALenum ALTE_API al_get_last_error_impl();

} // namespace AltE