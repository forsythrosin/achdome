#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <wormTracker.h>
#include <wormHead.h>
#include <wormCollision.h>
#include <wormArc.h>
#include <collisionSpace.h>
#include <renderSpace.h>
#include <wormEventListener.h>
#include <wormHeadDistributor.h>
#include <iostream>
#include <gamePlayer.h>
#include <gameConfig.h>

WormTracker::WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace, WormHeadDistributor *distributor, GameConfig *gameConfig) {
  this->collisionSpace = collisionSpace;
  this->renderSpace = renderSpace;
  this->distributor = distributor;
  this->gameConfig = gameConfig;

  std::random_device rd;
  randomGenerator = std::mt19937(rd());
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
  float width = gameConfig->wormWidth;
  clearPlayers();
  for (GamePlayer *gp : gamePlayers) {
    int id = gp->getId();
    WormHead *wh = new WormHead(id, gameConfig);
    wh->setWidth(width);
    wormHeads.insert({id, wh});
    setNewRandomGapTimer(id);
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
    glm::dvec3 cartesianPos = wormHead->getPosition();
    double phi = atan2(cartesianPos.y, cartesianPos.x);
    double theta = acos(cartesianPos.z);
    return glm::vec2(phi, theta);
  }
  return glm::vec2(0.0);
}

void WormTracker::tick(int time) {

  std::vector<WormArc> arcs;
  std::vector<WormHead> heads;

  for (const auto &pair : wormHeads) {
    int id = pair.first;
    WormHead *wh = pair.second;
    if (wh->isMoving()) {
      glm::dquat prevPosition = wh->getQuaternionPosition();
      wh->tick();
      glm::dquat position = wh->getQuaternionPosition();
      if (!wh->isInGap()) {
        arcs.push_back(WormArc(id, prevPosition, position, wh->getWidth(), time));
      }
      if (wh->needsNewGapTimer()) {
        setNewRandomGapTimer(id);
      }
    }
    heads.push_back(*wh);
  }

  std::vector<WormCollision> collisions = collisionSpace->addArcs(arcs);
  
  renderSpace->addHeads(heads);
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
  collisionSpace->clear();
  renderSpace->clear();
  renderSpace->reset();
}

bool WormTracker::startWormHead(int id) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second->start();
    for (WormEventListener *wel : eventListeners) {
      wel->onWormStarted(*(it->second));
    }
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


bool WormTracker::setNewRandomGapTimer(int id) {

  std::uniform_real_distribution<> timeBetweenGapsDistribution(gameConfig->minTimeBetweenGaps, gameConfig->maxTimeBetweenGaps);
  std::uniform_real_distribution<> timeInGapDistribution(gameConfig->minTimeInGap, gameConfig->maxTimeInGap);

  int ticksBetweenGaps = timeBetweenGapsDistribution(randomGenerator);
  int ticksInGap = timeInGapDistribution(randomGenerator);
  return setNewGapTimer(id, ticksBetweenGaps, ticksInGap);
}


bool WormTracker::setNewGapTimer(int id, int ticksBetweenGaps, int ticksInGap) {
  auto it = wormHeads.find(id);
  if (it != wormHeads.end()) {
    it->second->setGapTimer(ticksBetweenGaps, ticksInGap);
    return true;
  }  
  return false;
}

void WormTracker::saveCollisionBitmapToFile(std::string filename) {
  collisionSpace->saveBitmapToFile(filename);
}
