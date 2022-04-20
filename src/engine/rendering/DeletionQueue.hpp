#pragma once

#include <deque>
#include <functional>

namespace AltE {
  struct DeletionQueue {
      std::deque<std::function<void()>> deletors;

      void push_function(std::function<void()> &&function) {
        deletors.push_back(function);
      }

      void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
          (*it)(); // call functors
        }

        deletors.clear();
      }
  };
} // namespace AltE