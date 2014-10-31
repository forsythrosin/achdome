#pragma once

#include <vector>
class WormHead;
class GameConfig;

class WormHeadDistributor {
public:
  WormHeadDistributor(GameConfig *gameConfig) { this->gameConfig = gameConfig; };
  virtual ~WormHeadDistributor() {};
  virtual bool distribute(std::map<int, WormHead*> wormHeads) = 0;
protected:
  GameConfig *gameConfig;
};