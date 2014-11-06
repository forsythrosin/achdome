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
  Game(int gameId, PlayerManager *pm, WormTracker *wt);
  ~Game();
  int getId();
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
  std::vector<int> getParticipants();
  glm::vec2 getPosition(int playerId); // phi, theta
  int getPoints(int playerId);
  int getNumberOfPlayers();
  int getNumberOfPlayersAlive();
  int getNumberOfPlayersStartedMoving();
  int getNumberOfPlayersMoving();
  void tick();
  bool isOver();
 private:
  WormTracker *wormTracker;
  PlayerManager *playerManager;
  std::map<int, GamePlayer*> gamePlayers;
  void onWormCollision(WormCollision wc);
  void onPlayerDisconnect(int playerId);
  int time = 0;
  int gameId;
  bool gameOver = false;
  void updateGameOver();
};
