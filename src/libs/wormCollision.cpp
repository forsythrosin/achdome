#include <wormCollision.h>

WormCollision::WormCollision(int collider, int collidee, glm::quat position) {
  this->collider = collider;
  this->collidee = collidee;
  this->position = position;
}
