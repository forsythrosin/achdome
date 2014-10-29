#pragma once

#include <clusterState.h>
#include <wormArc.h>

class RenderableDome;
class RenderableWormGroup;
class ClusterRenderSpace;

class GameClusterState : public ClusterState {
 public:
  GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig);
  GameClusterState(sgct::Engine *gEngine, ClusterRenderSpace *rs, GameConfig *gameConfig);
  ~GameClusterState();

  void attach();
  void detach();

  void preSync();
  void draw();
  void encode();
  void decode();

 private:
  // Renderer + renderables
  RenderableDome *dome;
  RenderableWormGroup *worms;
  int domeGrid, domeWorms, wormLines;
  float timer = 0.0f;
  int stitchStep = 0;
  ClusterRenderSpace *renderSpace;

  sgct::SharedVector<WormArc> *wormArcs;
};
