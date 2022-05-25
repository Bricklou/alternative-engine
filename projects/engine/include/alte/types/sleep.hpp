#pragma once

#include <alte/export.hpp>

namespace AltE {
  class Time;

  /**
   * @brief Make the current thread sleep for a given duration
   *
   * sf::sleep is the best way to block a program or one of its  threads, as it
   * doesn't consume any CPU power. Compared to the standard
   * std::this_thread::sleep_for function, this one provides more accurate
   * sleeping time thanks to some platform-specific tweaks.
   *
   * @param duration Time to sleep
   */
  void ALTE_API sleep(Time duration);
} // namespace AltE