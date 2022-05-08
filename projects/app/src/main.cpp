
#include <alte/core/logger.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdio.h>

#include "entity/Player.hpp"
#include <alte/objects/ECS.hpp>
#include <alte/objects/GameObject.hpp>

struct TransformComponent {
  public:
    glm::vec3 get_position() { return position; }
    glm::vec3 get_rotation() { return rotation; }

    void set_position(glm::vec3 position) { this->position = position; }
    void set_rotation(glm::vec3 rotation) { this->rotation = rotation; }

  private:
    glm::vec3 position;
    glm::vec3 rotation;
};

int main(int argc, char *argv[]) {
  AltE::Core::Logger::create_logger("alternative-engine");

  spdlog::info("Hello world!");

  auto player = AltE::ECS::instance()->create_gameobject();

  auto &transform = player.add_component<TransformComponent>();

  transform.set_position({1, 2, 3});

  spdlog::info("Player position: {}", transform.get_position());
}