#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <collisionSpace.h>
#include <vector>
#include <bitmap.h>

class WormArc;
class WormCollision;

class FisheyeCollisionSpace : public CollisionSpace {
 public:
  /**
   * Constructor.
   */
  FisheyeCollisionSpace(int radius);

  /**
   * Destructor.
   */
  ~FisheyeCollisionSpace();

  /**
   * Add arcs to this collision space.
   */
  std::vector<WormCollision> addArcs(std::vector<WormArc> arcs);

  /**
   * Clear the collision space
   */
  void clear();

 private:
  int size;
  Bitmap *bitmap;
  glm::vec2 transform(glm::quat in);
  std::vector<glm::quat> getArcPoints(WormArc arc, double start = 0.0, double end = 1.0);
  static const float HALF_PIXEL;

};
