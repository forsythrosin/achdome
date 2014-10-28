#include <syncMaster.h>
#include <gameEngine.h>
#include <clusterState.h>


SyncMaster::SyncMaster(GameEngine *ge, std::map<GameEngine::State, ClusterState*> map) {
  gameEngine = ge;
  stateMap = map;
}

SyncMaster::~SyncMaster() {
  // Nothing to destroy.
}

ClusterState* SyncMaster::getClusterState() {
  GameEngine::State gameEngineState = gameEngine->getGameState();
  if (stateMap.find(gameEngineState) != stateMap.end()) {
    return stateMap[gameEngineState];
  }
  return nullptr;
}


void SyncMaster::detachAll() {
  for (auto it : stateMap) {
    ClusterState *s = it.second;
    if (s->isAttached()) {
      s->detach();
    }
  }
}

void SyncMaster::preSync() {
  if (ClusterState *cs = getClusterState()) {
    cs->preSync();
  }
}

void SyncMaster::encode() {
  if (ClusterState *cs = getClusterState()) {
    cs->encode();
  }
}


void SyncMaster::decode() {
  if (ClusterState *cs = getClusterState()) {
    cs->decode();
  }
}

void SyncMaster::draw() {
  if (ClusterState *cs = getClusterState()) {
    if (!cs->isAttached()) {
      detachAll();
      cs->attach();
    }
    cs->draw();
  }
}
