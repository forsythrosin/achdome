#include <glm/glm.hpp>
#include <vector>
#include <map>

class CollisionSpace;
class RenderSpace;
class WormHead;

class WormTracker {
 public:
  WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace);
  ~WormTracker();
  bool createWormHead(int id, glm::vec3 eulerPosition, glm::vec3 eulerRotation);
  void tick();

 private:
  std::map<int, WormHead> wormHeads;
  CollisionSpace *collisionSpace;
  RenderSpace *renderSpace;
};
