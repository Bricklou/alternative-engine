#pragma once

#include "alte/export.hpp"
#include <glm/glm.hpp>

namespace AltE {
  struct ALTE_API Transformable {
    public:
      void set_position(const glm::vec3 &position);
      void set_rotation(const glm::vec3 &rotation);

      glm::vec3 get_position() const { return _position; }
      glm::vec3 get_rotation() const { return _rotation; }

    private:
      glm::vec3 _position;
      glm::vec3 _rotation;
  };
} // namespace AltE