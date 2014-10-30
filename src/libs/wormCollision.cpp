#include <wormCollision.h>

WormCollision::WormCollision(int collider, int collidee, glm::quat position) {
  this->collider = collider;
  this->collidee = collidee;
  this->position = position;
}

glm::vec3 WormCollision::getCartesianPosition() {
  return glm::mat3_cast(glm::normalize(position)) * glm::vec3(1.0, 0.0, 0.0);
}
