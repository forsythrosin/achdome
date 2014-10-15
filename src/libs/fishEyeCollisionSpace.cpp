#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <fisheyeCollisionSpace.h>
#include <wormCollision.h>
#include <wormArc.h>

FisheyeCollisionSpace::FisheyeCollisionSpace(int radius) {
  
}

FisheyeCollisionSpace::~FisheyeCollisionSpace() {

}

std::vector<WormCollision> FisheyeCollisionSpace::addArcs(std::vector<WormArc> arcs) {
  

  std::vector<WormCollision> v;
  return v;
}

void FisheyeCollisionSpace::clear() {

}

glm::vec2 FisheyeCollisionSpace::transform(glm::quat in) {
  glm::vec3 pos = glm::mat3_cast(in) * glm::vec3(1.0, 0.0, 0.0);

  float phi = atan(pos.y/pos.x);
  float theta = acos(pos.z);

  return glm::vec2(phi, theta);
}
