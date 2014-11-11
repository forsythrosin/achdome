#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class WormArc {
 public:
  WormArc(int id, glm::dquat firstQuat, glm::dquat secondQuat, float width, int time);
  glm::dvec3 getCartesianLerp(double t);
  glm::dquat getFirstQuat();
  glm::dquat getSecondQuat();
  glm::dquat getLerp(double t);
  glm::vec4 getColor();
  int getWormId();
  float getWidth();
  int getTime();
 private:
  float width;
  int time;
  int wormId;
  glm::dquat firstQuat;
  glm::dquat secondQuat;
  glm::vec4 color;
};
