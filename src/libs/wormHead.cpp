#include <wormHead.h>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <random>

const int WormHead::MIN_TIME_BETWEEN_GAPS = 80;
const int WormHead::MAX_TIME_BETWEEN_GAPS = 100;
const int WormHead::GAP_TIME = 5;

/**
 * Construct a worm head
 */
WormHead::WormHead(glm::vec3 pos, glm::vec3 vel) {
  setEulerPosition(pos);
  setEulerVelocity(vel);

  std::random_device rd;
  randomGenerator = std::mt19937(rd());
  gapDistribution = std::uniform_int_distribution<>(MIN_TIME_BETWEEN_GAPS, MAX_TIME_BETWEEN_GAPS);
  
  float angle = 0.01;
  glm::vec3 axis(0, 0, 1);
  glm::vec3 normalizedAxis = ((float)sin(angle/2.0)) * axis;
  turnSpeed = 0.1;

  setGapTimer();
}

/**
 * Move worm head forward
 */
void WormHead::tick() {
  glm::vec3 pos = getPosition();
  if (turningLeft) {
    float turnAngle = -turnSpeed;
    glm::vec3 normalizedAxis = pos*((float)sin(turnAngle/2.0));
    glm::quat turnLeftQuat(cos(turnAngle/2.0), normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);

    float angle = acos(velocityQuat.w)*2;
    glm::vec3 axis = glm::normalize(glm::axis(velocityQuat));

    glm::vec3 newAxis = glm::mat3_cast(turnLeftQuat) * axis;
    glm::vec3 normalizedNewAxis = ((float)sin(angle/2.0))*newAxis;

    velocityQuat = glm::quat(cos(angle/2.0), normalizedNewAxis.x, normalizedNewAxis.y, normalizedNewAxis.z);
  }
  if (turningRight) {
    float turnAngle = turnSpeed;
    glm::vec3 normalizedAxis = pos*((float)sin(turnAngle/2.0));
    glm::quat turnRightQuat(cos(turnAngle/2.0), normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);

    float angle = acos(velocityQuat.w)*2;
    glm::vec3 axis = glm::normalize(glm::axis(velocityQuat));

    glm::vec3 newAxis = glm::mat3_cast(turnRightQuat) * axis;
    glm::vec3 normalizedNewAxis = ((float)sin(angle/2.0))*newAxis;
    
     velocityQuat = glm::quat(cos(angle/2), normalizedNewAxis.x, normalizedNewAxis.y, normalizedNewAxis.z);
  }

  if (isMoving()) {
    positionQuat = velocityQuat * positionQuat;
    gapTimer--;

    if (gapTimer < -GAP_TIME) {
      setGapTimer();
    }
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


bool WormHead::isMoving() const {
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

void WormHead::setGapTimer() {
  gapTimer = gapDistribution(randomGenerator);
}

bool WormHead::isInGap() {
  return gapTimer < 0;
}
