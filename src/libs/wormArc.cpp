#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <wormArc.h>
#include <iostream>

WormArc::WormArc(int id, glm::quat firstQuat, glm::quat secondQuat) {
  this->wormId = id;
  this->firstQuat = firstQuat;
  this->secondQuat = secondQuat;
}

glm::vec3 WormArc::getCartesianLerp(float t) {
  // glm::vec3 v1 = glm::mat3_cast(glm::normalize(firstQuat)) * glm::vec3(1.0, 0.0, 0.0);
  // glm::vec3 v2 = glm::mat3_cast(glm::normalize(secondQuat)) * glm::vec3(1.0, 0.0, 0.0);

  // float angle = acos(glm::dot(v1, v2))*t;
  // glm::vec3 rotationAxis = glm::cross(v1, v2);

  // // Quaterion normalization
  // rotationAxis *= sin(angle/2.0);
  // float w = cos(angle/2.0);

  // glm::quat interpolatedQuat(w, rotationAxis.x, rotationAxis.y, rotationAxis.z);

  // return glm::mat3_cast(glm::normalize(interpolatedQuat)) * v1;

  glm::quat interpolatedQuat = glm::slerp(firstQuat, secondQuat, t);
  return glm::mat3_cast(glm::normalize(interpolatedQuat)) * glm::vec3(1.0, 0.0, 0.0);
}
