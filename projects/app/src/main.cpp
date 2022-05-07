
#include <alte/core/logger.hpp>
#include <iostream>
#include <spdlog/spdlog.h>
#include <stdio.h>

#include "entity/Player.hpp"

int main(int argc, char *argv[]) {
  AltE::Core::Logger::create_logger("alternative-engine");

  spdlog::debug("Hello world!");

  Player player{};

  player.say_hello();
  player.set_position({1.0f, 2.0f, 3.0f});

  spdlog::info("Player position: {}", player.get_position());
}