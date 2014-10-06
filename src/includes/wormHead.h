#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

class WormHead {
  bool alive;
  glm::quat positionQuat;
  glm::quat velocityQuat;
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
};
