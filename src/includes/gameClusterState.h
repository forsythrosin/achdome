#pragma once

#include <clusterState.h>
#include <uniformWrappers.h>
#include <deque>
#include <wormHeadSyncData.h>
#include <wormArcSyncData.h>
#include <wormCollisionSyncData.h>
#include <wormEventListener.h>

class RenderableDome;
class RenderableWormArcs;
class RenderableWormHeads;
class ClusterRenderSpace;
class GameEngine;
class WormArcSyncData;
class WormHeadAppearance;

class WormCollisionSyncData;
class WormHead;
class WormTracker;

class GameClusterState : public ClusterState, public WormEventListener {
 public:
  GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig);
  GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs, GameEngine *ge, WormTracker *wt);
  ~GameClusterState();

  void attach();
  void detach();
  void reset();

  void preSync();
  void postSyncPreDraw();
  void draw();
  void encode();
  void decode();
  void onWormStarted(WormHead head);

 private:
  // Renderer + renderables
  RenderableDome *renderableDome;
  RenderableWormArcs *renderableArcs;
  RenderableWormHeads *renderableHeads;

  int domeGrid, domeWorms, wormLines, wormDots, collision;
  int stitchStep = 0;
  ClusterRenderSpace *renderSpace;
  GameEngine *gameEngine;
  WormTracker *wormTracker;
  std::deque<std::pair<int, WormCollisionSyncData> > collisionTimerQueue;
  std::map<int, WormHeadAppearance*> wormHeadAppearances;
  Uniform<float> *timeUni;
  Uniform<std::vector<glm::vec4> > *collisionsUni;
  Uniform<GLuint> *collisionCountUni;
  WormHeadAppearance* getWormHeadAppearance(WormHead wh);
  
  sgct::SharedBool *resetSignal;
  sgct::SharedVector<WormArcSyncData> *wormArcs;
  sgct::SharedVector<WormCollisionSyncData> *wormCollisions;
  sgct::SharedVector<WormHeadSyncData> *wormHeads;
  sgct::SharedInt timer;

  const GLuint COLLISION_DURATION = 100; // frames
};
