
#include <alte/core/logger.hpp>
#include <cmath>
#include <iostream>

#include "alte/audio/SoundStream.hpp"
#include "alte/types/time.hpp"
#include "alte/types/time.inl"
#include "entity/Player.hpp"
#include "spdlog/spdlog.h"
#include <alte/ECS/AudioComponent.hpp>
#include <alte/ECS/AudioListenerComponent.hpp>
#include <alte/ECS/TransformComponent.hpp>
#include <alte/audio/Music.hpp>
#include <alte/objects/ECS.hpp>
#include <alte/objects/GameObject.hpp>
#include <alte/types/sleep.hpp>

void run() {
  AltE::Core::Logger::create_logger("alternative-engine");

  SPDLOG_INFO("Hello world!");

  auto player = AltE::ECS::instance()->create_gameobject();

  auto &transform = player.add_component<AltE::TransformComponent>();
  auto &listener = player.add_component<AltE::AudioListenerComponent>();
  // auto &audio = player.add_component<AltE::AudioComponent>();

  AltE::AudioComponent audio{};
  AltE::Music music{};

  if (!music.open_from_file("assets/audio/Perk Pietrek & Abstrakt - Pressure "
                            "[NCS Release]_mono.wav")) {
    throw std::runtime_error("Failed to open audio file");
  }

  audio.set_source(music);
  transform.set_position({0, 0, 0});
  audio.play();

  SPDLOG_INFO("Player position: {}", transform.get_position());

  uint32_t i = 0;
  float x = 0, y = 0;
  while (audio.get_status() == AltE::Music::Playing) {
    i = (i + 1) % 360;

    x = std::cos((360 - i) * M_PI / 180) * 20;
    y = std::sin((360 - i) * M_PI / 180) * 20;

    music.set_position({x, y, 0});

    SPDLOG_INFO("Position: {}", music.get_position());
    AltE::sleep(AltE::milliseconds(20));
  }
}

int main(int argc, char *argv[]) {
  try {
    run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}