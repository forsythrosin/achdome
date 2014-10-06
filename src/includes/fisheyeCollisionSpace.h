#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <collisionSpace.h>
#include <vector>

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
  virtual std::vector<WormCollision> addArcs(std::vector<WormArc> arcs);

  /**
   * Clear the collision space
   */
  void clear();

 private:
  glm::vec2 transform(glm::quat in);
};
