#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

class WormHead {
  glm::quat positionQuat;
  glm::quat velocityQuat;
 public:
  WormHead(glm::vec3 p, glm::vec3 v);
  void move();
  std::string toString();
  void setEulerPosition(glm::vec3 pos);
  void setEulerVelocity(glm::vec3 pos);
  glm::vec3 getPosition();
  glm::vec3 getVelocity();
};
