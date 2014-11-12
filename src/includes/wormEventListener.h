#pragma once

#include <wormCollision.h>
#include <wormHead.h>

class WormEventListener {
 public:
  virtual ~WormEventListener(){};
  virtual void onWormCollision(WormCollision wc) {};
  virtual void onWormStarted(WormHead wh) {};
};
