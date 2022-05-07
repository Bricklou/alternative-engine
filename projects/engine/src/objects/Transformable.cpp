#include <alte/objects/Transformable.hpp>

namespace AltE {

  void Transformable::set_position(const glm::vec3 &position) {
    this->_position = position;
  }

  void Transformable::set_rotation(const glm::vec3 &rotation) {
    this->_rotation = rotation;
  }

} // namespace AltE