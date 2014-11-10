#pragma once

class PlayerEventListener {
 public:
  virtual ~PlayerEventListener(){};
  virtual void onPlayerDisconnect(int playerId) = 0;
};
