#pragma once

#include "sgct.h"
#include <renderer.h>
#include <gameConfig.h>
#include <gameEngine.h>

class ClusterState {
 public:
  struct SubState;
  ClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : gEngine(gEngine), gameConfig(gameConfig){
    renderer = new Renderer(gEngine);
  };
  virtual ~ClusterState() {
    delete renderer;
  };

  void attach(GameEngine::State state){
    if(substateMap.count(state) > 0){
      currentSubState = substateMap[state];
    }
    attach();
  }

  SubState* const getActiveSubState(){
    return currentSubState;
  }

  virtual void attach() = 0;
  virtual void detach() = 0;

  virtual void preSync() = 0;
  virtual void postSyncPreDraw(){};
  virtual void draw() = 0;
  virtual void encode() = 0;
  virtual void decode() = 0;

  bool isAttached() { return attached; }

  struct SubState{
    virtual ~SubState(){};
    virtual void preSync(){};
    virtual void postSyncPreDraw(){};
    virtual void draw(){};
    virtual void encode(){};
    virtual void decode(){};
  };

 protected:
  bool attached;
  Renderer *renderer;
  sgct::Engine *gEngine;
  GameConfig *gameConfig;

  std::map<GameEngine::State, SubState*> substateMap;
  SubState *currentSubState = nullptr;

};
