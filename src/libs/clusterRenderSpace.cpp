#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <clusterRenderSpace.h>
#include <wormArc.h>
#include <wormCollision.h>
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

void ClusterRenderSpace::addCollisions(std::vector<WormCollision> arcs) {

}

void ClusterRenderSpace::clear() {
  this->arcs.clear();
}

std::vector<WormArc> ClusterRenderSpace::getArcs() {
  return this->arcs;
}
