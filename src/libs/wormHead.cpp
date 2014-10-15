#include <wormHead.h>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cmath>

/**
 * Construct a worm head
 */
WormHead::WormHead(glm::vec3 pos, glm::vec3 vel) {
  setEulerPosition(pos);
  setEulerVelocity(vel);
  turnSpeed = 0.1;
}

/**
 * Move worm head forward
 */
void WormHead::tick() {
  glm::vec3 pos = getPosition();
  if (turningLeft) {
    float angle = turnSpeed;
    glm::vec3 normalizedAxis = pos*((float)sin(angle/2.0));
    glm::quat turnLeftQuat(cos(angle/2), normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);
    velocityQuat = velocityQuat * turnLeftQuat;
  }
  if (turningRight) {
    float angle = -turnSpeed;
    glm::vec3 normalizedAxis = pos*((float)sin(angle/2));
    glm::quat turnRightQuat(cos(angle/2), normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);
    velocityQuat = velocityQuat * turnRightQuat;
  }
  if (isMoving()) {
    positionQuat = positionQuat * velocityQuat;
  }
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


bool WormHead::isMoving() {
  return moving;
}


void WormHead::stop() {
  moving = false;
}

void WormHead::start() {
  moving = true;
}

void WormHead::turnLeft(bool turn) {
  turningLeft = turn;
}

void WormHead::turnRight(bool turn) {
  turningRight = turn;  
}
