#pragma once

#include "sgct.h"
#include <gameEngine.h>
#include <map>

class ClusterState;

class SyncMaster {
public:
  SyncMaster(GameEngine *ge, std::map<GameEngine::State, ClusterState*> map);
  SyncMaster();
  ~SyncMaster();

  void preSync();
  void postSyncPreDraw();
  void encode();
  void decode();
  void draw();

  void setInitDone();

  void setGameEngine(GameEngine *ge);
  void setStateMap(std::map<GameEngine::State, ClusterState*> map);

private:
  void detachAll();
  void attachState(ClusterState *cs);
  ClusterState* getClusterState();

  GameEngine *gameEngine;
  sgct::SharedInt state;
  bool initDone;
  std::map<GameEngine::State, ClusterState*> stateMap;
};
