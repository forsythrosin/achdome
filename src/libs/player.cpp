#include <glm/glm.hpp>
#include <player.h>
#include <map>

Player::Player(int id, glm::vec4 color, std::string name) {
  this->id = id;
  this->color = color;
  const size_t size = std::extent<decltype(this->name)>::value;
  std::fill(this->name, this->name+size, 0);
  std::copy(name.begin(), name.end(), this->name);
  this->connected = true;
}


int Player::getId() {
  return id;
}

void Player::setName(std::string name) {
  std::fill(this->name, this->name + MAX_NAME_LENGTH, 0);
  std::copy(
    name.begin(),
    name.length() > MAX_NAME_LENGTH ? name.begin() + MAX_NAME_LENGTH : name.end(),
    this->name
  );
}

std::string Player::getName() {
  return this->name;
}

void Player::setColor(glm::vec4 color) {
  this->color = color;
}

glm::vec4 Player::getColor() {
  return this->color;
}

bool Player::isConnected() {
  return connected;
}


bool Player::disconnect() {
  connected = false;
  return true;
}

void Player::setPointsInGame(int gameId, int points) {
  this->points[gameId] = points;
}

int Player::getPointsInGame(int gameId) {
  if (points.find(gameId) == points.end()) {
    return -1;
  }
  return points[gameId];
}
