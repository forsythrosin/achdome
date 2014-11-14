#include <wormHead.h>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>
#include <random>
#include <gameConfig.h>

/**
 * Construct a worm head
 */
WormHead::WormHead(int id, GameConfig *gameConfig) {
  wormId = id;
  setEulerPosition(glm::dvec3(0.0));
  setEulerVelocity(glm::dvec3(0.0));
  this->gameConfig = gameConfig;

  this->width = gameConfig->wormWidth;
  moving = false;
  turningLeft = false;
  turningRight = false;
  turnSpeed = 0.1;

  timeLeftInGap = 0;
  timeLeftBetweenGaps = 0;
}

int WormHead::getWormId() {
  return wormId;
}

/**
 * Move worm head forward
 */
void WormHead::tick() {
  glm::dvec3 pos = getPosition();
  if (turningLeft) {
    double turnAngle = -gameConfig->turnSpeed;
    glm::dvec3 normalizedAxis = pos*(sin(turnAngle / 2.0));
    glm::dquat turnLeftQuat(cos(turnAngle/2.0), normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);

    double angle = acos(velocityQuat.w) * 2;
    glm::dvec3 axis = glm::normalize(glm::axis(velocityQuat));

    glm::dvec3 newAxis = glm::mat3_cast(turnLeftQuat) * axis;
    glm::dvec3 normalizedNewAxis = (sin(angle / 2.0))*newAxis;

    velocityQuat = glm::dquat(cos(angle/2.0), normalizedNewAxis.x, normalizedNewAxis.y, normalizedNewAxis.z);
  }
  if (turningRight) {
    double turnAngle = gameConfig->turnSpeed;
    glm::dvec3 normalizedAxis = pos*(sin(turnAngle/2.0));
    glm::dquat turnRightQuat(cos(turnAngle/2.0), normalizedAxis.x, normalizedAxis.y, normalizedAxis.z);

    double angle = acos(velocityQuat.w) * 2;
    glm::dvec3 axis = glm::normalize(glm::axis(velocityQuat));

    glm::dvec3 newAxis = glm::mat3_cast(turnRightQuat) * axis;
    glm::dvec3 normalizedNewAxis = (sin(angle/2.0))*newAxis;

    velocityQuat = glm::dquat(cos(angle/2), normalizedNewAxis.x, normalizedNewAxis.y, normalizedNewAxis.z);
  }

  if (isMoving()) {
    positionQuat = velocityQuat * positionQuat;
    if (isInGap()) {
      timeLeftInGap--;
    } else {
      timeLeftBetweenGaps--;
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
glm::dvec3 WormHead::getPosition() {
  return glm::mat3_cast(positionQuat) * glm::dvec3(1.0, 0.0, 0.0);
}

/**
 * Get velocity in 3D space
 */
glm::dvec3 WormHead::getVelocity() {
  glm::dquat nextPosQuat = velocityQuat * positionQuat;
  glm::dvec3 posVec = glm::mat3_cast(positionQuat) *glm::dvec3(1.0, 0.0, 0.0);
  glm::dvec3 nextPosVec = glm::mat3_cast(nextPosQuat) *glm::dvec3(1.0, 0.0, 0.0);
  return nextPosVec - posVec;
}

/**
* Set position
*/
void WormHead::setPosition(glm::dquat pos) {
  positionQuat = pos;
}

/**
* Set velocity
*/
void WormHead::setVelocity(glm::dquat vel) {
  velocityQuat = vel;
}



/** * Set position from Euler angles (XYZ)
 */
void WormHead::setEulerPosition(glm::dvec3 pos) {
  positionQuat = glm::dquat(pos);
}

/**
 * Set velocity from Euler angles (XYZ)
 */
void WormHead::setEulerVelocity(glm::dvec3 vel) {
  velocityQuat = glm::dquat(vel);
}



/**
 * Get position in quaternions
 */
glm::dquat WormHead::getQuaternionPosition() {
  return positionQuat;
}

/**
 * Get velocity in quaternions
 */
glm::dquat WormHead::getQuaternionVelocity() {
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

bool WormHead::isInGap() {
  return timeLeftBetweenGaps <= 0;
}

bool WormHead::needsNewGapTimer() {
  return timeLeftBetweenGaps <= 0 && timeLeftInGap <= 0;
}


void WormHead::setGapTimer(int ticksBetweenGaps, int ticksInGap) {
  timeLeftBetweenGaps = ticksBetweenGaps;
  timeLeftInGap = ticksInGap;
}

void WormHead::setWidth(float width) {
  this->width = width;
}

float WormHead::getWidth() {
  return gameConfig->wormWidth;
}
