#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class WormArc {
 public:
  WormArc(int id, glm::dquat firstQuat, glm::dquat secondQuat, int time, glm::vec4 color);
  glm::dvec3 getCartesianLerp(double t);
  glm::dquat getLerp(double t);
  glm::vec4 getColor();
  int getWormId();
  int getTime();
 private:
  int time;
  int wormId;
  glm::dquat firstQuat;
  glm::dquat secondQuat;
  glm::vec4 color;
};
