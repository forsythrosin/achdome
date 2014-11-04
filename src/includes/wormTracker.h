#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <random>

class CollisionSpace;
class RenderSpace;
class WormHead;
class WormEventListener;
class WormHeadDistributor;
class GamePlayer;
class GameConfig;

class WormTracker {
 public:
  WormTracker(CollisionSpace *collisionSpace, RenderSpace *renderSpace, WormHeadDistributor *distributor, GameConfig *gameConfig);
  ~WormTracker();
  void tick(int time);
  void clear();
  bool startWormHead(int id);
  bool stopWormHead(int id);
  bool turnLeft(int id, bool turn);
  bool turnRight(int id, bool turn);
  void addEventListener(WormEventListener *we);
  void removeEventListener(WormEventListener *we);
  void setPlayers(std::vector<GamePlayer*> players);
  glm::vec2 getSphericalPosition(int playerId);
  void clearPlayers();
  void saveCollisionBitmapToFile(std::string filename);
 private:
  std::map<int, WormHead*> wormHeads;
  CollisionSpace *collisionSpace;
  RenderSpace *renderSpace;
  WormHeadDistributor *distributor;
  GameConfig *gameConfig;
  std::vector<WormEventListener*> eventListeners;
  bool setNewRandomGapTimer(int id);
  bool setNewGapTimer(int id, int ticksBetweenGaps, int ticksInGap);
  std::mt19937 randomGenerator;
  
  const int MIN_TIME_BETWEEN_GAPS = 100;
  const int MAX_TIME_BETWEEN_GAPS = 200;
  const int MIN_TIME_IN_GAP = 20;
  const int MAX_TIME_IN_GAP = 30;
};
