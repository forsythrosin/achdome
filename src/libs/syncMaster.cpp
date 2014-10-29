#include <syncMaster.h>
#include <gameEngine.h>
#include <clusterState.h>


SyncMaster::SyncMaster(GameEngine *ge, std::map<GameEngine::State, ClusterState*> map) {
  gameEngine = ge;
  stateMap = map;
  state.setVal(GameEngine::INTRO);
}

SyncMaster::~SyncMaster() {
  // Nothing to destroy.
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

  if (ClusterState *cs = getClusterState()) {
    cs->preSync();
  }
}

void SyncMaster::encode() {
  GameEngine::State gameEngineState = gameEngine->getGameState();
  state.setVal(gameEngineState);

  sgct::SharedData::instance()->writeInt(&state);

  if (ClusterState *cs = getClusterState()) {
    attachState(cs);
    cs->encode();
  }
}

void SyncMaster::decode() {
  std::cout << "before" << std::endl;
  sgct::SharedData::instance()->readInt(&state);
  std::cout << "after" << std::endl;

  if (ClusterState *cs = getClusterState()) {
    attachState(cs);
    cs->decode();
  }
}

void SyncMaster::draw() {
  if (ClusterState *cs = getClusterState()) {
    attachState(cs);
    cs->draw();
  }
}
