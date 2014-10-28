#include <glm/glm.hpp>
#include <vector>
#include <map>

class CollisionSpace;
class RenderSpace;
class WormHead;
class WormEventListener;
class WormHeadDistributor;

class WormTracker {
 public:
  WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace, WormHeadDistributor *distributor);
  ~WormTracker();
  void tick(int time);
  void clear();
  bool startWormHead(int id);
  bool stopWormHead(int id);
  bool turnLeft(int id, bool turn);
  bool turnRight(int id, bool turn);
  void addEventListener(WormEventListener *we);
  void removeEventListener(WormEventListener *we);
  void setPlayers(std::vector<int> playerIds);
  glm::vec2 getSphericalPosition(int playerId);
  void clearPlayers();
 private:
  std::map<int, WormHead*> wormHeads;
  CollisionSpace *collisionSpace;
  RenderSpace *renderSpace;
  WormHeadDistributor *distributor;
  std::vector<WormEventListener*> eventListeners;
};
