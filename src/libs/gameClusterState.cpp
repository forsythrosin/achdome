#include <gameClusterState.h>

#include <renderableDome.h>
#include <renderableWormGroup.h>
#include <clusterRenderSpace.h>

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : GameClusterState(gEngine, gameConfig, nullptr) {}

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs) : ClusterState(gEngine, gameConfig) {
  wormArcs = new sgct::SharedVector<WormArc>(gameConfig->maximumPlayers);
  wormCollisions = new sgct::SharedVector<WormCollision>(gameConfig->maximumPlayers);
  wormHeads = new sgct::SharedVector<WormHead>(gameConfig->maximumPlayers);

  renderSpace = rs;
  attached = false;
}

GameClusterState::~GameClusterState() {
  if(dome){
    delete dome;
  }
  if(worms){
    delete worms;
  }
  if(wormArcs){
    delete wormArcs;
  }
  if(wormCollisions){
    delete wormCollisions;
  }
  if(wormHeads){
    delete wormHeads;
  }
}

void GameClusterState::attach() {

  dome = new RenderableDome(50, 20);
  worms = new RenderableWormGroup(gameConfig->maximumPlayers, 4, gameConfig->lineWidth);

  domeGrid = renderer->addRenderable(dome, GL_LINES, "domeShader.vert", "domeGridShader.frag", true);
  domeWorms = renderer->addRenderable(dome, GL_TRIANGLES, "domeShader.vert", "domeWormsShader.frag", true);
  collision = renderer->addRenderable(dome, GL_LINES, "domeShader.vert", "collisionShader.frag", true);

  worms->setWormArcs(wormArcs->getVal());
  wormLines = renderer->addRenderable(worms, GL_TRIANGLES, "wormShader.vert", "wormShader.frag", false);

  attached = true;
}

void GameClusterState::detach() {
  renderSpace->clear();
  wormArcs->setVal({});

  renderer->removeRenderable(domeGrid);
  renderer->removeRenderable(domeWorms);
  renderer->removeRenderable(collision);
  renderer->removeRenderable(wormLines);

  delete dome;
  delete worms;

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
