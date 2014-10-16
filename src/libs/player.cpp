#include <glm/glm.hpp>
#include <player.h>


Player::Player(int id, glm::vec3 color, std::string name) {
  this->id = id;
  this->color = color;
  this->name = name;
  this->connected = true;
}


int Player::getId() {
  return id;
}

void Player::setName(std::string name) {
  this->name = name;
}

std::string Player::getName() {
  return this->name;
}

bool Player::isConnected() {
  return connected;
}


bool Player::disconnect() {
  connected = false;
  return true;
}
