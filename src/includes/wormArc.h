#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class WormArc {
 public:
  WormArc(int id, glm::quat firstQuat, glm::quat secondQuat);
  glm::vec3 getCartesianLerp(float t);
  glm::quat getLerp(float t);
  int getWormId();
  int getTime();
 private:
  int time;
  int wormId;
  glm::quat firstQuat;
  glm::quat secondQuat;
};
