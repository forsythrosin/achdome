#include <wormHeadAppearance.h>

WormHeadAppearance::WormHeadAppearance(glm::vec4 color, float diameterCoefficient, float strokeWidth, float arrowLength, float arrowWidth) {
  this->color = color;
  this->diameterCoefficient = diameterCoefficient;
  this->strokeWidth = strokeWidth;
  this->arrowLength = arrowLength;
  this->arrowWidth = arrowWidth;
}

float WormHeadAppearance::getDiameterCoefficient() {
  return diameterCoefficient;
}

float WormHeadAppearance::getStrokeWidth() {
  return strokeWidth;
}

float WormHeadAppearance::getArrowLength() {
  return arrowLength;
}

float WormHeadAppearance::getArrowWidth() {
  return arrowWidth;
}

glm::vec4 WormHeadAppearance::getColor() {
  return color;
}
