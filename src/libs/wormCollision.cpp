#include <wormCollision.h>

WormCollision::WormCollision(int collider, int collidee, glm::dquat position) {
  this->collider = collider;
  this->collidee = collidee;
  this->position = position;
}

glm::dvec3 WormCollision::getCartesianPosition() {
  return glm::mat3_cast(glm::normalize(position)) * glm::dvec3(1.0, 0.0, 0.0);
}
