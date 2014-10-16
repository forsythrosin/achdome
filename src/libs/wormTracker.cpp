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
#include <wormEventListener.h>

WormTracker::WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace) {
  this->collisionSpace = collisionSpace;
  this->renderSpace = renderSpace;
}

WormTracker::~WormTracker() {

}

void WormTracker::addEventListener(WormEventListener *wel) {
  eventListeners.push_back(wel);
}

void WormTracker::removeEventListener(WormEventListener *wel) {
  for (int i = 0; i < eventListeners.size(); i++) {
    if (eventListeners[i] == wel) {
      eventListeners.erase(eventListeners.begin() + i);
    }
  }
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
    if (wh.isMoving()) {
      glm::quat prevPosition = wh.getQuaternionPosition();
      wh.tick();
      glm::quat position = wh.getQuaternionPosition();
      arcs.push_back(WormArc(id, prevPosition, position));
    }
  }

  std::vector<WormCollision> collisions = collisionSpace->addArcs(arcs);

  renderSpace->addArcs(arcs);
  renderSpace->addCollisions(collisions);

  for (WormEventListener *wel : eventListeners) {
    for (WormCollision c : collisions) {
      wel->onWormCollision(c);
    }
  }
}

void WormTracker::clear() {
  wormHeads.clear();
}

bool WormTracker::startWormHead(int id) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second.start();
	return true;
  }
  return false;
}

bool WormTracker::stopWormHead(int id) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second.stop();
	return true;
  }
  return false;
}

bool WormTracker::turnLeft(int id, bool turn) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second.turnLeft(turn);
	return true;
  }
  return false;
}

bool WormTracker::turnRight(int id, bool turn) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second.turnRight(turn);
	return true;
  }
  return false;
}
