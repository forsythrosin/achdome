#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class WormArc {
 public:
  WormArc(int id, glm::quat firstPoint, glm::quat secondPoint);
 private:
  int wormId;
  glm::quat firstPoint;
  glm::quat secondPoint;

};
