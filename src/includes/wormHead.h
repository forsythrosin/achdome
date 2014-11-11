#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

class GameConfig;

class WormHead {
 public:
  WormHead(int id, GameConfig *gameConfig);
  void tick();
  std::string toString();

  /**
   * Set position using an Euler Angle vector (x, y, z)
   */
  void setEulerPosition(glm::dvec3 pos);

  /**
   * Set velocity using an Euler Angle vector (x, y, z)
   */
  void setEulerVelocity(glm::dvec3 pos);

  /**
   * Get worm id.
   */
  int getWormId();

  /**
   * Get position in cartesian coordinates
   */
  glm::dvec3 getPosition();

  /**
   * Get velocity in cartesian coordinates
   */
  glm::dvec3 getVelocity();

  /**
  * Set position
  */
  void setPosition(glm::dquat);

  /**
  * Set velocity
  */
  void setVelocity(glm::dquat);

  /**
   * Get position in quaternions
   */
  glm::dquat getQuaternionPosition();

  /**
   * Get velocity in quaternions
   */
  glm::dquat getQuaternionVelocity();
  
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
  
  /**
   * Set width.
   */
  void setWidth(float width);

  /**
   * Get width.
   */
  float getWidth();

 private:
  int wormId;
  bool moving;
  bool turningLeft;
  bool turningRight;
  glm::dquat positionQuat;
  glm::dquat velocityQuat;
  double turnSpeed;
  double width;
  int timeLeftBetweenGaps;
  int timeLeftInGap;
  GameConfig *gameConfig;
};
