#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <wormArc.h>

WormArc::WormArc(int id, glm::quat firstPoint, glm::quat secondPoint) {
  this->wormId = id;
  this->firstPoint = firstPoint;
  this->secondPoint = secondPoint;
}
