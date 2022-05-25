#include "Player.hpp"
#include <spdlog/spdlog.h>

Player::Player() { SPDLOG_INFO("Player::Player()"); }

Player::~Player() {}

void Player::say_hello() { SPDLOG_INFO("Player: Hello world!"); }