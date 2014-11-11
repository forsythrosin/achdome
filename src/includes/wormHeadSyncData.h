#pragma once

#include <wormHead.h>
#include <glm/glm.hpp>
#include <wormHeadAppearance.h>

class WormHeadSyncData {
public:
  WormHeadSyncData(WormHead wh, WormHeadAppearance wha);
  glm::vec4 getColor();
  glm::dvec3 getPosition();
  glm::dvec3 getVelocity();
  float getHeadDiameter();
  float getStrokeWidth();
  float getArrowWidth();
  float getArrowLength();
private:
  glm::vec4 color;
  glm::dvec3 position;
  glm::dvec3 velocity;
  float headDiameter;
  float strokeWidth;
  float arrowWidth;
  float arrowLength;
};
