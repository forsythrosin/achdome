#pragma once

#include <glm/glm.hpp>

class WormHeadAppearance {
 public:
  WormHeadAppearance(glm::vec4 color, float diameterCoefficient, float strokeWidth, float arrowLength, float arrowWidth);
  float getDiameterCoefficient();
  float getArrowLength();
  float getArrowWidth();
  float getStrokeWidth();
  glm::vec4 getColor();
 private:
  float diameterCoefficient;
  float arrowLength;
  float arrowWidth;
  float strokeWidth;
  glm::vec4 color;
};
