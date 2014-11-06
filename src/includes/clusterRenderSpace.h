#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <renderSpace.h>
#include <vector>

class WormArc;
class WormCollision;
class WormHead;

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
   * Add worm arcs.
   */
  void addHeads(std::vector<WormHead>);

  /**
   * Add collisions.
   */
  void addCollisions(std::vector<WormCollision>);

  /**
   * Reset the render space. (Set signal to reset FBO.)
   */
  void reset();
  
  /**
   * Clear the render space
   */
  void clear();

  /**
   * Get arcs
   */
  std::vector<WormArc> getArcs();

  /**
   * Get collisions
   */
  std::vector<WormCollision> getCollisions();

  /**
   * Get heads
   */
  std::vector<WormHead> getHeads();

  /**
   * Get reset signal
   */
  bool getResetSignal();

 private:
  std::vector<WormArc> arcs;
  std::vector<WormCollision> collisions;
  std::vector<WormHead> heads;
  bool resetSignal = false;
};
