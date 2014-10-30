#include <gameClusterState.h>

#include <renderable.h>
#include <renderer.h>
#include <renderableDome.h>
#include <renderableWormGroup.h>
#include <clusterRenderSpace.h>
#include <iostream>

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : GameClusterState(gEngine, gameConfig, nullptr) {}

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs) : ClusterState(gEngine, gameConfig) {
  wormArcs = new sgct::SharedVector<WormArc>(MAX_NUMBER_OF_WORMS);
  wormCollisions = new sgct::SharedVector<WormCollision>(MAX_NUMBER_OF_WORMS);
  wormHeads = new sgct::SharedVector<WormHead>(MAX_NUMBER_OF_WORMS);
  renderSpace = rs;
  attached = false;
}

GameClusterState::~GameClusterState() {
  delete dome;
  delete worms;
  delete wormArcs;
}

void GameClusterState::attach() {
  dome = new RenderableDome(50, 20);
  domeWorms = renderer->addRenderable(dome, GL_TRIANGLES, "domeShader.vert", "domeWormsShader.frag", true);
  domeGrid = renderer->addRenderable(dome, GL_LINES, "domeShader.vert", "domeGridShader.frag", true);

  worms = new RenderableWormGroup(2, 4, gameConfig->lineWidth);
  worms->setWormArcs(wormArcs->getVal());

  wormLines = renderer->addRenderable(worms, GL_TRIANGLES, "wormShader.vert", "wormShader.frag", false);
  attached = true;
}

void GameClusterState::detach() {
  renderer->removeRenderable(domeWorms);
  renderer->removeRenderable(domeGrid);
  renderer->removeRenderable(wormLines);
  attached = false;
}

void GameClusterState::preSync() {
  if (renderSpace != nullptr) {
    std::vector<WormArc> arcs = renderSpace->getArcs();
    std::vector<WormCollision> collisions = renderSpace->getCollisions();
    std::vector<WormHead> heads = renderSpace->getHeads();
    //    std::vector<WormCollision> collisions = renderSpace->getCollisions();

    wormArcs->setVal(arcs);
    wormCollisions->setVal(collisions);
    wormHeads->setVal(heads);

    //    std::cout << arcs.size() << std::endl;
    //    wormCollisions->setVal(collisions);

    renderSpace->clear();
  }

  // Reset stitch step
  stitchStep = 0;
}

void GameClusterState::draw() {
  // Copy current worm positions and colors
  std::vector<WormArc> arcs = wormArcs->getVal();

  worms->setWormArcs(arcs);

  // render wormLines to FBO
  renderer->renderToFBO(wormLines, stitchStep);
  // render grid lines
  renderer->render(domeGrid);
  // render FBO as texture on dome
  renderer->render(domeWorms, wormLines, stitchStep);

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
