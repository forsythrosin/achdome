#pragma once

#include <clusterState.h>
#include <wormArc.h>
#include <wormCollision.h>
#include <wormHead.h>

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

  void preSync();
  void draw();
  void encode();
  void decode();

 private:
  // Renderer + renderables
  RenderableDome *renderableDome;
  RenderableWormArcs *renderableArcs;
  RenderableWormHeads *renderableHeads;

  int domeGrid, domeWorms, wormLines, wormDots, collision;
  float timer = 0.0f;
  int stitchStep = 0;
  ClusterRenderSpace *renderSpace;

  sgct::SharedVector<WormArc> *wormArcs;
  sgct::SharedVector<WormCollision> *wormCollisions;
  sgct::SharedVector<WormHead> *wormHeads;
};
