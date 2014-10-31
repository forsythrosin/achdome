#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <collisionSpace.h>
#include <vector>
#include <bitmap.h>
#include <string>

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

  /**
   * Save bitmap to file
   */
  void saveBitmapToFile(std::string filename);

 private:
  int size;
  Bitmap *bitmap;
  glm::dvec2 transform(glm::dquat in);
  std::vector<glm::dquat> getArcPoints(WormArc arc, double start = 0.0, double end = 1.0);
  static const float HALF_PIXEL;
};
