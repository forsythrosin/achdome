#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

class WormArc;
class WormCollision;

// Render space interface



class RenderSpace {
 public:
  /**
   * Destructor.
   */
  virtual ~RenderSpace() {};

  /**
   * Add worm arcs.
   */
  virtual void addArcs(std::vector<WormArc>) = 0;

  /**
   * Add collisions.
   */
  virtual void addCollisions(std::vector<WormCollision>) = 0;

  /**
   * Clear the render space
   */
  virtual void clear() = 0;
};
