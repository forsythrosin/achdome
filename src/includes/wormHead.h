#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <random>

class WormHead {
 public:
  WormHead(glm::vec3 p, glm::vec3 v);
  void tick();
  std::string toString();

  /**
   * Set position using an Euler Angle vector (x, y, z)
   */
  void setEulerPosition(glm::vec3 pos);

  /**
   * Set velocity using an Euler Angle vector (x, y, z)
   */
  void setEulerVelocity(glm::vec3 pos);

  /**
   * Get position in cartesian coordinates
   */
  glm::vec3 getPosition();

  /**
   * Get velocity in cartesian coordinates
   */
  glm::vec3 getVelocity();

  /**
   * Get position in quaternions
   */
  glm::quat getQuaternionPosition();

  /**
   * Get velocity in quaternions
   */
  glm::quat getQuaternionVelocity();
  
  /**
   * Turn left.
   */
  void turnLeft(bool turn);

  /**
   * Turn right.
   */
  void turnRight(bool turn);

  /**
   * Is moving?
   */
  bool isMoving() const;

  /**
   * Stop.
   */
  void stop();

  /**
   * Start.
   */
  void start();

  /**
   * Is in gap-state
   */
  bool isInGap();

 private: 
  bool moving;
  bool turningLeft;
  bool turningRight;
  glm::quat positionQuat;
  glm::quat velocityQuat;
  float turnSpeed;
  void setGapTimer();
  int gapTimer;
  static const int MIN_TIME_BETWEEN_GAPS;
  static const int MAX_TIME_BETWEEN_GAPS;
  static const int GAP_TIME;
  std::mt19937 randomGenerator;
  std::uniform_int_distribution<> gapDistribution;
};
