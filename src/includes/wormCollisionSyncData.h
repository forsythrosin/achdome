#pragma once

#include <wormCollision.h>

class WormCollisionSyncData {
 public:
  WormCollisionSyncData(WormCollision wc, glm::vec4 color);
  glm::vec4 getColor();
  float getTime();
  glm::vec3 getPosition();
 private:
  glm::vec4 color;
  float time;
  glm::vec3 position;
};
