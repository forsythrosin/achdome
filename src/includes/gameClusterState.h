#pragma once

#include <clusterState.h>
#include <wormArc.h>
#include <wormCollision.h>
#include <wormHead.h>
#include <uniformWrappers.h>
#include <deque>

class RenderableDome;
class RenderableWormArcs;
class RenderableWormHeads;
class ClusterRenderSpace;

class GameClusterState : public ClusterState {
 public:
  GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig);
  GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs);
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
  std::deque<std::pair<int, WormCollision> > collisionTimerQueue;
  Uniform<float> *timeUni;
  Uniform<std::vector<glm::vec4> > *collisionsUni;
  Uniform<GLuint> *collisionCountUni;

  sgct::SharedBool *resetSignal;
  sgct::SharedVector<WormArc> *wormArcs;
  sgct::SharedVector<WormCollision> *wormCollisions;
  sgct::SharedVector<WormHead> *wormHeads;
  sgct::SharedInt timer;

  const GLuint COLLISION_DURATION = 100; // frames
};
