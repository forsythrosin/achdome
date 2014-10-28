#pragma once

#include "sgct.h"
#include <gameEngine.h>
#include <map>

class ClusterState;

class SyncMaster {
 public: 
  SyncMaster(GameEngine *ge, std::map<GameEngine::State, ClusterState*>);
  ~SyncMaster();
  void preSync();
  void encode();
  void decode();
  void draw();
 private:
  GameEngine *gameEngine;
  sgct::SharedObject<GameEngine::State> state;
  std::map<GameEngine::State, ClusterState*> stateMap;
  ClusterState *getClusterState();
  void detachAll();
};
