#include <wormArcSyncData.h>
#include <glm/glm.hpp>

WormArcSyncData::WormArcSyncData(WormArc wa, glm::vec4 color) {
  this->color = color;
  this->firstQuat = wa.getFirstQuat();
  this->secondQuat = wa.getSecondQuat();
  this->time = wa.getTime();
  this->width = wa.getWidth();
}

glm::vec4 WormArcSyncData::getColor() {
  return color;
}

glm::dvec3 WormArcSyncData::getCartesianLerp(double t) {
  glm::dquat interpolatedQuat = glm::slerp(firstQuat, secondQuat, t);
  return glm::mat3_cast(glm::normalize(interpolatedQuat)) * glm::dvec3(1.0, 0.0, 0.0);
}

float WormArcSyncData::getWidth() {
  return width;
}
