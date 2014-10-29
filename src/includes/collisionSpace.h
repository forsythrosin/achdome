#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class WormCollision;
class WormArc;

// Collision space interface

class CollisionSpace {
 public:
  /**
   * Destructor.
   */
  virtual ~CollisionSpace() {};

  /**
   * Add arcs to this collision space.
   */
  virtual std::vector<WormCollision> addArcs(std::vector<WormArc> arcs) = 0;

  /**
   * Clear the collision space
   */
  virtual void clear() = 0;
};
