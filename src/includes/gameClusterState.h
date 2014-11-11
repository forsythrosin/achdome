#pragma once

#include <clusterState.h>
#include <uniformWrappers.h>
#include <deque>
#include <wormHeadSyncData.h>
#include <wormArcSyncData.h>
#include <wormCollisionSyncData.h>

class RenderableDome;
class RenderableWormArcs;
class RenderableWormHeads;
class ClusterRenderSpace;
class PlayerManager;
class WormArcSyncData;
class WormHeadAppearance;

class WormCollisionSyncData;
class WormHead;

class GameClusterState : public ClusterState {
 public:
  GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig);
  GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs, PlayerManager *pm);
  ~GameClusterState();

  void attach();
  void detach();
  void reset();

  void preSync();
  void postSyncPreDraw();
  void draw();
  void encode();
  void decode();

 private:
  // Renderer + renderables
  RenderableDome *renderableDome;
  RenderableWormArcs *renderableArcs;
  RenderableWormHeads *renderableHeads;

  int domeGrid, domeWorms, wormLines, wormDots, collision;
  int stitchStep = 0;
  ClusterRenderSpace *renderSpace;
  PlayerManager *playerManager;
  std::deque<std::pair<int, WormCollisionSyncData> > collisionTimerQueue;
  std::map<int, WormHeadAppearance*> wormHeadAppearances;
  Uniform<float> *timeUni;
  Uniform<std::vector<glm::vec4> > *collisionsUni;
  Uniform<std::vector<glm::vec4> > *collisionColorsUni;
  Uniform<GLuint> *collisionCountUni;

  sgct::SharedBool *resetSignal;
  sgct::SharedVector<WormArcSyncData> *wormArcs;
  sgct::SharedVector<WormCollisionSyncData> *wormCollisions;
  sgct::SharedVector<WormHeadSyncData> *wormHeads;
  sgct::SharedInt timer;

  const GLuint COLLISION_DURATION = 100; // frames
};
