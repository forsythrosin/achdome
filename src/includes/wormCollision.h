#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class WormCollision {
 public:
  WormCollision(int collider, int collidee, glm::dquat position);
  glm::dvec3 getCartesianPosition();
  int collider, collidee;
  glm::dquat position;
};
