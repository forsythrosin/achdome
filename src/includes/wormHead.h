#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

class WormHead {
 public:
  WormHead(glm::vec4 color);
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
  * Get color
  */
  glm::vec4 getColor();

  /**
   * Get position in cartesian coordinates
   */
  glm::vec3 getPosition();

  /**
   * Get velocity in cartesian coordinates
   */
  glm::vec3 getVelocity();

  /**
  * Set position
  */
  void setPosition(glm::quat);

  /**
  * Set velocity
  */
  void setVelocity(glm::quat);

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

  /**
   * Needs new gap timer
   */
  bool needsNewGapTimer();

  /**
   * Set gap timer.
   */
  void setGapTimer(int ticksBetweenGaps, int ticksInGap);
  
 private:
  glm::vec4 color;
  bool moving;
  bool turningLeft;
  bool turningRight;
  glm::quat positionQuat;
  glm::quat velocityQuat;
  float turnSpeed;
  int timeLeftBetweenGaps;
  int timeLeftInGap;
};
