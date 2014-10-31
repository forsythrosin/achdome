#pragma once

#include <map>
#include <random>
#include <wormHeadDistributor.h>

class GameConfig;

class UniformDistributor : public WormHeadDistributor {
public:
  UniformDistributor(GameConfig *gameConfig);
  ~UniformDistributor();
  bool distribute(std::map<int, WormHead*> wormHeads);
private:
  std::mt19937 randGen;
};