#pragma once

#include <vector>

class Player;

class GamePlayer {
 public:
  GamePlayer(Player *p);
  std::vector<int> getKillIds();
  int getKiller();
  int getId();
  bool isAlive();
  bool startMoving();
  bool kill(int killerId);
  bool addKill(int playerId);
  bool hasKilled(int playerId);
 private:
  bool alive;
  bool startedMoving;
  Player *player;
  std::vector<int> kills;
  int killer;
};
