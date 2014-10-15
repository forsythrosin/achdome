#include <wormHead.h>
#include <sstream>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

/**
 * Construct a worm head
 */
WormHead::WormHead(glm::vec3 pos, glm::vec3 vel) {
  setEulerPosition(pos);
  setEulerVelocity(vel);
}

/**
 * Move worm head forward
 */
void WormHead::tick() {
  positionQuat = positionQuat * velocityQuat;
}

/**
 * Get a string describing the
 */
std::string WormHead::toString() {
  std::stringstream ss;

  ss << "WormHead: { pos: " << glm::to_string(getPosition()) << " ";

  ss << "posQuat: (" <<
    positionQuat.w << ", " <<
    positionQuat.x << ", " <<
    positionQuat.y << ", " <<
    positionQuat.z << "), ";

  ss << "velQuat: (" <<
    velocityQuat.w << ", " <<
    velocityQuat.x << ", " <<
    velocityQuat.y << ", " <<
    velocityQuat.z << ")}";

  return ss.str();
}

/**
 * Get position in 3D space
 */
glm::vec3 WormHead::getPosition() {
  return glm::mat3_cast(positionQuat) * glm::vec3(1.0, 0.0, 0.0);
}

/**
 * Get velocity in 3D space
 */
glm::vec3 WormHead::getVelocity() {
  return glm::mat3_cast(velocityQuat) * glm::vec3(1.0, 0.0, 0.0);
}



/** * Set position from Euler angles (XYZ)
 */
void WormHead::setEulerPosition(glm::vec3 pos) {
  positionQuat = glm::quat(pos);
}

/**
 * Set velocity from Euler angles (XYZ)
 */
void WormHead::setEulerVelocity(glm::vec3 vel) {
  velocityQuat = glm::quat(vel);
}



/**
 * Get position in quaternions
 */
glm::quat WormHead::getQuaternionPosition() {
  return positionQuat;
}

/**
 * Get velocity in quaternions
 */
glm::quat WormHead::getQuaternionVelocity() {
  return velocityQuat;
}
