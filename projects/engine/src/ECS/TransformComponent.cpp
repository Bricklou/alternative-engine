#include <alte/ECS/TransformComponent.hpp>

namespace AltE {

  void TransformComponent::set_position(const glm::vec3 &position) {
    this->_position = position;
  }

  void TransformComponent::set_rotation(const glm::vec3 &rotation) {
    this->_rotation = rotation;
  }

} // namespace AltE