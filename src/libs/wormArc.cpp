#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <wormArc.h>
#include <iostream>

WormArc::WormArc(int id, glm::dquat firstQuat, glm::dquat secondQuat, float width, int time, glm::vec4 color) {
  this->wormId = id;
  this->firstQuat = firstQuat;
  this->secondQuat = secondQuat;
  this->width = width;
  this->time = time;
  this->color = color;
}

float WormArc::getWidth() {
  return width;
}

int WormArc::getWormId() {
  return wormId;
}

int WormArc::getTime() {
  return time;
}

glm::dvec3 WormArc::getCartesianLerp(double t) {
  glm::dquat interpolatedQuat = glm::slerp(firstQuat, secondQuat, t);
  return glm::mat3_cast(glm::normalize(interpolatedQuat)) * glm::dvec3(1.0, 0.0, 0.0);
}

glm::dquat WormArc::getLerp(double t) {
  return glm::slerp(firstQuat, secondQuat, t);
}

glm::vec4 WormArc::getColor() {
  return color;
}
