#pragma once

#include <wormEventListener.h>
#include <playerEventListener.h>
#include <wormCollision.h>
#include <map>
#include <vector>



class PlayerManager;
class WormTracker;
class GamePlayer;

class Game : WormEventListener, PlayerEventListener {
 public:
  Game(PlayerManager *pm, WormTracker *wt);
  bool turnLeft(int playerId, bool turn);
  bool turnRight(int playerId, bool turn);
  bool startMoving(int playerId);
  bool hasStartedMoving(int playerId);
  bool isAlive(int playerId);
  bool isParticipating(int playerId);
  int getKiller(int playerId);
  std::vector<int> getKills(int playerId);
  bool start();
  bool end();
  std::string getCountry(int playerId);
  glm::vec2 getPosition(int playerId); // phi, theta
  void tick();
  
 private:
  WormTracker *wormTracker;
  std::map<int, GamePlayer*> gamePlayers;
  void onWormCollision(WormCollision wc);
  void onPlayerDisconnect(int playerId);
};
