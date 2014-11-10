#pragma once

class WormCollision;

class WormEventListener {
 public:
  virtual ~WormEventListener(){};
  virtual void onWormCollision(WormCollision wc) = 0;
};
