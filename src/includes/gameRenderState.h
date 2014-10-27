#pragma once

#include <renderState.h>
#include <wormArc.h>

class RenderableDome;
class RenderableWormGroup;

class GameRenderState : public RenderState {
 public:
  GameRenderState(sgct::Engine *gEngine);
  ~GameRenderState();

  void init();
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

  sgct::SharedVector<WormArc> *wormArcs;
};