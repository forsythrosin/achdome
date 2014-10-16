#include <glm/glm.hpp>
#include <vector>
#include <map>

class CollisionSpace;
class RenderSpace;
class WormHead;
class WormEventListener;

class WormTracker {
 public:
  WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace);
  ~WormTracker();
  bool createWormHead(int id, glm::vec3 eulerPosition, glm::vec3 eulerRotation);
  void tick();
  void clear();
  bool startWormHead(int id);
  bool stopWormHead(int id);
  bool turnLeft(int id, bool turn);
  bool turnRight(int id, bool turn);
  void addEventListener(WormEventListener *we);
  void removeEventListener(WormEventListener *we);
 private:
  std::map<int, WormHead*> wormHeads;
  CollisionSpace *collisionSpace;
  RenderSpace *renderSpace;
  std::vector<WormEventListener*> eventListeners;
};
