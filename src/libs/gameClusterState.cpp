#include <gameClusterState.h>

#include <renderable.h>
#include <renderer.h>
#include <renderableDome.h>
#include <renderableWormArcs.h>
#include <renderableWormHeads.h>
#include <clusterRenderSpace.h>
#include <iostream>

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : GameClusterState(gEngine, gameConfig, nullptr) {}

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs) : ClusterState(gEngine, gameConfig) {
  wormArcs = new sgct::SharedVector<WormArc>(gameConfig->maximumPlayers);
  wormCollisions = new sgct::SharedVector<WormCollision>(gameConfig->maximumPlayers);
  wormHeads = new sgct::SharedVector<WormHead>(gameConfig->maximumPlayers);

  renderSpace = rs;
  attached = false;
}

GameClusterState::~GameClusterState() {
  delete renderableDome;
  delete renderableArcs;
  delete renderableHeads;

  delete wormArcs;
  delete wormCollisions;
  delete wormHeads;
}

void GameClusterState::attach() {

  renderableDome = new RenderableDome(50, 20);
  renderableArcs = new RenderableWormArcs(gameConfig->maximumPlayers, 4, gameConfig->lineWidth);
  renderableHeads = new RenderableWormHeads(gameConfig->maximumPlayers, gameConfig->lineWidth);

  domeGrid = renderer->addRenderable(renderableDome, GL_LINES, "domeShader.vert", "domeGridShader.frag", true);
  domeWorms = renderer->addRenderable(renderableDome, GL_TRIANGLES, "domeShader.vert", "domeWormsShader.frag", true);
  collision = renderer->addRenderable(renderableDome, GL_LINES, "domeShader.vert", "collisionShader.frag", true);

  renderableArcs->setWormArcs(wormArcs->getVal());
  wormLines = renderer->addRenderable(renderableArcs, GL_TRIANGLES, "wormShader.vert", "wormShader.frag", false);
  renderableHeads->setWormHeads(wormHeads->getVal());
  wormDots = renderer->addRenderable(renderableHeads, GL_TRIANGLES, "wormShader.vert", "wormShader.frag", false);

  attached = true;
}

void GameClusterState::detach() {
  renderSpace->clear();
  wormArcs->setVal({});

  renderer->removeRenderable(domeGrid);
  renderer->removeRenderable(domeWorms);
  renderer->removeRenderable(collision);
  renderer->removeRenderable(wormLines);
  renderer->removeRenderable(wormDots);

  delete renderableDome;
  delete renderableArcs;
  delete renderableHeads;

  attached = false;
}

void GameClusterState::preSync() {
  if (renderSpace != nullptr) {
    std::vector<WormArc> arcs = renderSpace->getArcs();
    std::vector<WormHead> heads = renderSpace->getHeads();
    std::vector<WormCollision> collisions = renderSpace->getCollisions();

    wormArcs->setVal(arcs);
    wormCollisions->setVal(collisions);
    wormHeads->setVal(heads);

    renderSpace->clear();
  }

  // Reset stitch step
  stitchStep = 0;
}

void GameClusterState::draw() {
  // Copy current worm positions and colors
  std::vector<WormArc> arcs = wormArcs->getVal();
  std::vector<WormHead> heads = wormHeads->getVal();
  std::vector<WormCollision> collisions = wormCollisions->getVal();
  renderableArcs->setWormArcs(arcs);
  renderableHeads->setWormHeads(heads);

  renderer->render(wormDots);
  // render wormLines to FBO
  renderer->renderToFBO(wormLines, stitchStep);
  // render grid lines
  renderer->render(domeGrid);
  // render FBO as texture on dome
  renderer->render(domeWorms, wormLines, stitchStep);

  // renderer->render(collision);

  ++stitchStep;
}

void GameClusterState::encode() {
  // get things from renderSpace and send it to everyone.
  sgct::SharedData *data = sgct::SharedData::instance();

  data->writeVector(wormArcs);
  data->writeVector(wormCollisions);
  data->writeVector(wormHeads);
}

void GameClusterState::decode() {
  // read from buffer and insert data to GameRenderers.
  sgct::SharedData *data = sgct::SharedData::instance();

  data->readVector(wormArcs);
  data->readVector(wormCollisions);
  data->readVector(wormHeads);
}
