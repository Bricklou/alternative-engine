#pragma once

#include <alte/export.hpp>
#include <alte/objects/ECS.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entt.hpp>

namespace AltE {

  class ALTE_API GameObject {
    public:
      GameObject(entt::entity handle, ECS *ecs);
      GameObject(const GameObject &other) = default;

      template <typename T> bool has_component();

      template <typename T, typename... Targs>
      T &add_component(Targs &&...args);

      template <typename T> T &get_component();

      template <typename T> void remove_component();

      operator bool() const { return _entity_handle == entt::null; }

    private:
      entt::entity _entity_handle{entt::null};
      ECS *_ecs;
  };
} // namespace AltE

#include <alte/objects/GameObject-inl.hpp>