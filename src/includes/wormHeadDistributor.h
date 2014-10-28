#pragma once

#include <vector>
class WormHead;

class WormHeadDistributor {
public:
  virtual ~WormHeadDistributor() {};
  virtual bool distribute(std::map<int, WormHead*> wormHeads) = 0;
protected:
  const float velocityAngle = 0.01;
};