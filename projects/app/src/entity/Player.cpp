#include "Player.hpp"
#include "spdlog/spdlog.h"
#include <iostream>

Player::Player() : Entity{} { std::cout << "Player created" << std::endl; }

Player::~Player() {}

void Player::say_hello() { spdlog::info("Player: Hello world!"); }