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
  void detachAll();
  void attachState(ClusterState *cs);
  ClusterState* getClusterState();

  GameEngine *gameEngine;
  sgct::SharedInt state;
  std::map<GameEngine::State, ClusterState*> stateMap;
};
