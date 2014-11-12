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
  void tweenDiameterCoefficient(float destination, float duration);
  void tweenStrokeWidth(float destination, float duration);
  void tweenArrowWidth(float destination, float duration);
  void tweenArrowLength(float destination, float duration);
 private:
  float diameterCoefficient;
  float arrowLength;
  float arrowWidth;
  float strokeWidth;
  glm::vec4 color;
  int diameterTween = -1;
  int strokeWidthTween = -1;
  int arrowWidthTween = -1;
  int arrowLengthTween = -1;
};
