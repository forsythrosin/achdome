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
#include <wormHeadDistributor.h>
#include <iostream>
#include <gamePlayer.h>

WormTracker::WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace, WormHeadDistributor *distributor) {
  this->collisionSpace = collisionSpace;
  this->renderSpace = renderSpace;
  this->distributor = distributor;
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

void WormTracker::setPlayers(std::vector<GamePlayer*> gamePlayers) {
  clearPlayers();
  for (GamePlayer *gp : gamePlayers) {
    wormHeads.insert({gp->getId(), new WormHead(gp->getColor())});
  }
  distributor->distribute(wormHeads);
}

void WormTracker::clearPlayers() {
  for (const auto &pair : wormHeads) {
    delete pair.second;
  }
  wormHeads.clear();
}

glm::vec2 WormTracker::getSphericalPosition(int playerId) {
  if (wormHeads.count(playerId)) {
    WormHead *wormHead = wormHeads.at(playerId);
    glm::vec3 cartesianPos = wormHead->getPosition();
    float phi = atan2f(cartesianPos.y, cartesianPos.x);
    float theta = acosf(cartesianPos.z);
    return glm::vec2(phi, theta);
  }
  return glm::vec2(0.0);
}

void WormTracker::tick(int time) {

  std::vector<WormArc> arcs;

  for (const auto &pair : wormHeads) {
    int id = pair.first;
    WormHead *wh = pair.second;
    if (wh->isMoving()) {
      glm::quat prevPosition = wh->getQuaternionPosition();
      wh->tick();
      glm::quat position = wh->getQuaternionPosition();
      if (!wh->isInGap()) {
        arcs.push_back(WormArc(id, prevPosition, position, time, wh->getColor()));
      }
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
  for (auto it : wormHeads) {
    delete it.second;
  }
  wormHeads.clear();
}

bool WormTracker::startWormHead(int id) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second->start();
    return true;
  }
  return false;
}

bool WormTracker::stopWormHead(int id) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second->stop();
    return true;
  }
  return false;
}

bool WormTracker::turnLeft(int id, bool turn) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second->turnLeft(turn);
    return true;
  }
  return false;
}

bool WormTracker::turnRight(int id, bool turn) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second->turnRight(turn);
    return true;
  }
  return false;
}
