#include <alte/objects/ECS.hpp>

#include <alte/objects/GameObject.hpp>
#include <glm/glm.hpp>

namespace AltE {

  ECS::ECS() {}

  ECS::~ECS() {}

  GameObject ECS::create_gameobject() { return {_registry.create(), this}; }
} // namespace AltE