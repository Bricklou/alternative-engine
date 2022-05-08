#pragma once

#include "alte/export.hpp"
#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>
#include <stdint.h>
#include <vector>

namespace AltE {
  class GameObject;

  class ALTE_API ECS {
    public:
      using object_id = entt::entity;

      ECS();
      ~ECS();

      static ECS *instance() {
        static ECS *instance = new ECS();
        return instance;
      }

      GameObject create_gameobject();

    private:
      entt::registry _registry;

      friend class GameObject;
  };
} // namespace AltE