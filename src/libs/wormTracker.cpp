#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <wormTracker.h>
#include <wormHead.h>
#include <wormCollision.h>
#include <wormArc.h>
#include <collisionSpace.h>
#include <renderSpace.h>
#include <map>
#include <vector>


WormTracker::WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace) {
  this->collisionSpace = collisionSpace;
  this->renderSpace = renderSpace;
}

WormTracker::~WormTracker() {

}

bool WormTracker::createWormHead(int id, glm::vec3 eulerPosition, glm::vec3 eulerRotation) {
  if (wormHeads.find(id) == wormHeads.end()) {
    WormHead wh(eulerPosition, eulerRotation);
    wormHeads.insert(std::pair<int, WormHead>(id, wh));
    return true;
  }
  return false;
}

void WormTracker::tick() {

  std::vector<WormArc> arcs;

  for (const auto &pair : wormHeads) {
    int id = pair.first;
    WormHead wh = pair.second;
    glm::quat prevPosition = wh.getQuaternionPosition();
    wh.tick();
    glm::quat position = wh.getQuaternionPosition();
    arcs.push_back(WormArc(id, prevPosition, position));
  }

  std::vector<WormCollision> collisions = collisionSpace->addArcs(arcs);

  renderSpace->addArcs(arcs);
  renderSpace->addCollisions(collisions);
}
