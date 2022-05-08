#include <alte/objects/GameObject.hpp>

namespace AltE {
  GameObject::GameObject(entt::entity handle, ECS *ecs)
      : _entity_handle{handle}, _ecs{ecs} {}
} // namespace AltE