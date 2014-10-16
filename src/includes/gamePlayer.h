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
 private:
  bool alive;
  Player *player;
  std::vector<int> kills;
  int killer;
};
