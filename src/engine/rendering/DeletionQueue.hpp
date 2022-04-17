#pragma once

#include <deque>
#include <functional>

namespace AltE {
  struct DeletionQueue {
      std::deque<std::function<void()>> deletors;

      void push_function(std::function<void()> f) { deletors.push_back(f); }

      void flush() {
        // reverse iterate the deletion queue to execute all the functions
        for (auto it = deletors.rbegin(); it != deletors.rend(); ++it) {
          (*it)(); // call the function
        }

        deletors.clear();
      }
  };
} // namespace AltE