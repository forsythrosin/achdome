#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class WormCollision {
 public:
  WormCollision(int collider, int collidee, glm::quat position);
  int collider, collidee;
  glm::quat position;
};
