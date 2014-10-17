#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <renderSpace.h>
#include <vector>

class WormArc;
class WormCollision;

// Render space interface



class ClusterRenderSpace : public RenderSpace {
 public:
  /**
   * Constructor.
   */
  ClusterRenderSpace();

  /**
   * Destructor.
   */
  ~ClusterRenderSpace();

  /**
   * Add worm arcs.
   */
  void addArcs(std::vector<WormArc>);

  /**
   * Add collisions.
   */
  void addCollisions(std::vector<WormCollision>);

  /**
   * Clear the render space
   */
  void clear();

  /**
   * Get arcs
   */
  std::vector<WormArc> getArcs();

 private:
  std::vector<WormArc>arcs;
};
