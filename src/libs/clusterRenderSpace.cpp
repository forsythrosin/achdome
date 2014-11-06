#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <clusterRenderSpace.h>
#include <wormArc.h>
#include <wormCollision.h>
#include <wormHead.h>
#include <iostream>
ClusterRenderSpace::ClusterRenderSpace() {
  
}

ClusterRenderSpace::~ClusterRenderSpace() {

}

void ClusterRenderSpace::addArcs(std::vector<WormArc> arcs) {
  for (auto arc : arcs) {
    this->arcs.push_back(arc);
  }
}

void ClusterRenderSpace::addCollisions(std::vector<WormCollision> collisions) {
  for (auto collision : collisions) {
    this->collisions.push_back(collision);
  }
}

void ClusterRenderSpace::addHeads(std::vector<WormHead> heads) {
  for (auto head : heads) {
    this->heads.push_back(head);
  }
}

void ClusterRenderSpace::reset() {
  resetSignal = true;
}


void ClusterRenderSpace::clear() {
  this->arcs.clear();
  this->collisions.clear();
  this->heads.clear();
  resetSignal = false;
}

std::vector<WormArc> ClusterRenderSpace::getArcs() {
  return this->arcs;
}

std::vector<WormCollision> ClusterRenderSpace::getCollisions() {
  return this->collisions;
}

std::vector<WormHead> ClusterRenderSpace::getHeads() {
  return this->heads;
}

bool ClusterRenderSpace::getResetSignal() {
  return resetSignal;
}
