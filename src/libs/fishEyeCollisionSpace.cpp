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
  return glm::vec2(0, 0);
}
