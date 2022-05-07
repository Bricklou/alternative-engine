#pragma once

#include "alte/objects/Entity.hpp"

class Player : public AltE::Entity {
  public:
    Player();
    ~Player();

    void say_hello();
};