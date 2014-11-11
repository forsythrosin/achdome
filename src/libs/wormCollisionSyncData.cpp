#include <wormCollisionSyncData.h>

WormCollisionSyncData::WormCollisionSyncData(WormCollision wc, glm::vec4 color) {
  this->color = color;
  this->time = wc.time;
  this->position = wc.getCartesianPosition();
}

glm::vec4 WormCollisionSyncData::getColor() {
  return color;
}

float WormCollisionSyncData::getTime() {
  return time;
}

glm::vec3 WormCollisionSyncData::getPosition() {
  return position;
}
