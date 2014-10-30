#include <syncMaster.h>
#include <gameEngine.h>
#include <clusterState.h>


SyncMaster::SyncMaster(GameEngine *ge, std::map<GameEngine::State, ClusterState*> map) : SyncMaster() {
  gameEngine = ge;
  stateMap = map;
}

SyncMaster::SyncMaster() {
  state.setVal(1);
  initDone = false;
}

SyncMaster::~SyncMaster() {
  // Nothing to destroy.
}

void SyncMaster::setInitDone() {
  initDone = true;
}

void SyncMaster::setGameEngine(GameEngine *ge) {
  gameEngine = ge;
}

void SyncMaster::setStateMap(std::map<GameEngine::State, ClusterState*> map) {
  stateMap = map;
}

ClusterState* SyncMaster::getClusterState() {
  GameEngine::State gameState = static_cast<GameEngine::State>(state.getVal());
  if (stateMap.find(gameState) != stateMap.end()) {
    return stateMap[gameState];
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

void SyncMaster::attachState(ClusterState *cs) {
  if (!cs->isAttached()) {
    detachAll();
    cs->attach();
  }
}

void SyncMaster::preSync() {
  if (!initDone) return;

  if (ClusterState *cs = getClusterState()) {
    cs->preSync();
  }
}

void SyncMaster::encode() {
  if (!initDone) return;

  GameEngine::State gameEngineState = gameEngine->getGameState();
  state.setVal(gameEngineState);

  sgct::SharedData::instance()->writeInt(&state);

  if (ClusterState *cs = getClusterState()) {
    attachState(cs);
    cs->encode();
  }
}

void SyncMaster::decode() {
  if (!initDone) return;

  sgct::SharedData::instance()->readInt(&state);

  if (ClusterState *cs = getClusterState()) {
    attachState(cs);
    cs->decode();
  }
}

void SyncMaster::draw() {
  if (!initDone) return;

  if (ClusterState *cs = getClusterState()) {
    attachState(cs);
    cs->draw();
  }
}
