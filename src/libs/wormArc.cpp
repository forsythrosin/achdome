#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <wormArc.h>
#include <iostream>

WormArc::WormArc(int id, glm::quat firstQuat, glm::quat secondQuat, int time, glm::vec4 color) {
  this->wormId = id;
  this->firstQuat = firstQuat;
  this->secondQuat = secondQuat;
  this->time = time;
  this->color = color;
}

int WormArc::getWormId() {
  return wormId;
}

int WormArc::getTime() {
  return time;
}

glm::vec3 WormArc::getCartesianLerp(float t) {
  glm::quat interpolatedQuat = glm::slerp(firstQuat, secondQuat, t);
  return glm::mat3_cast(glm::normalize(interpolatedQuat)) * glm::vec3(1.0, 0.0, 0.0);
}

glm::quat WormArc::getLerp(float t) {
  return glm::slerp(firstQuat, secondQuat, t);
}

glm::vec4 WormArc::getColor() {
  return color;
}