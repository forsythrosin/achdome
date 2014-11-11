#pragma once

#include <wormArc.h>
#include <glm/glm.hpp>

class WormArcSyncData {
 public:
  WormArcSyncData(WormArc wa, glm::vec4 color);
  float getTime();
  float getWidth();
  glm::dvec3 getCartesianLerp(double t);
  glm::vec4 getColor();
 private:
  glm::vec4 color;
  glm::dquat firstQuat;
  glm::dquat secondQuat;
  float time;
  float width;
};
