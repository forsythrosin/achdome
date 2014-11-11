#include <wormHeadSyncData.h>

WormHeadSyncData::WormHeadSyncData(WormHead wh, WormHeadAppearance wha) {
  this->position = wh.getPosition();
  this->velocity = wh.getVelocity();

  this->headDiameter = wh.getWidth() * wha.getDiameterCoefficient();
  this->strokeWidth = wha.getStrokeWidth();
  this->arrowLength = wha.getArrowLength();
  this->arrowWidth = wha.getArrowWidth();
  this->color = wha.getColor();
}


glm::dvec3 WormHeadSyncData::getPosition() {
  return position;
} 

glm::dvec3 WormHeadSyncData::getVelocity() {
  return velocity;
} 

glm::vec4 WormHeadSyncData::getColor() {
  return color;
} 

float WormHeadSyncData::getStrokeWidth() {
  return strokeWidth;
} 

float WormHeadSyncData::getHeadDiameter() {
  return headDiameter;
} 

float WormHeadSyncData::getArrowWidth() {
  return arrowLength;
} 

float WormHeadSyncData::getArrowLength() {
  return arrowLength;
} 

