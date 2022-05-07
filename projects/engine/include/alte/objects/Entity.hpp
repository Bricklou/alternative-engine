#pragma once

#include "alte/export.hpp"
#include "alte/objects/Transformable.hpp"

namespace AltE {
  class ALTE_API Entity : public Transformable {
    public:
      Entity() = default;
      ~Entity() = default;

    private:
  };
} // namespace AltE