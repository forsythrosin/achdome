#pragma once

class WormCollision;

class WormEventListener {
 public:
  virtual void onWormCollision(WormCollision wc) = 0;
};
