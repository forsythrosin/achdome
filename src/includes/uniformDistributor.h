#pragma once

#include <map>
#include <random>
#include <wormHeadDistributor.h>

class UniformDistributor : public WormHeadDistributor {
public:
  UniformDistributor();
  ~UniformDistributor();
  bool distribute(std::map<int, WormHead*> wormHeads);
private:
  std::mt19937 randGen;
};