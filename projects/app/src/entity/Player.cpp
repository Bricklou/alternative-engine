#include "Player.hpp"
#include <iostream>
#include <spdlog/spdlog.h>

Player::Player() { spdlog::info("Player::Player()"); }

Player::~Player() {}

void Player::say_hello() { spdlog::info("Player: Hello world!"); }