#include <gameClusterState.h>
#include <renderableDome.h>
#include <renderableWormArcs.h>
#include <renderableWormHeads.h>
#include <clusterRenderSpace.h>

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : GameClusterState(gEngine, gameConfig, nullptr) {}

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs) : ClusterState(gEngine, gameConfig) {
  resetSignal = new sgct::SharedBool(false);
  wormArcs = new sgct::SharedVector<WormArc>(gameConfig->maximumPlayers);
  wormCollisions = new sgct::SharedVector<WormCollision>(gameConfig->maximumPlayers);
  wormHeads = new sgct::SharedVector<WormHead>(gameConfig->maximumPlayers);
  timer.setVal(0);

  renderableDome = nullptr;
  renderableArcs = nullptr;
  renderableHeads = nullptr;

  timeUni = nullptr;
  collisionsUni = nullptr;

  renderSpace = rs;
  attached = false;
}

GameClusterState::~GameClusterState() {
  if (renderableDome != nullptr) delete renderableDome;
  if (renderableArcs != nullptr) delete renderableArcs;
  if (renderableHeads != nullptr) delete renderableHeads;

  if (wormArcs != nullptr) delete wormArcs;
  if (wormCollisions != nullptr) delete wormCollisions;
  if (wormHeads != nullptr) delete wormHeads;

  if (timeUni != nullptr) delete timeUni;
  if (collisionsUni != nullptr) delete collisionsUni;
}

void GameClusterState::attach() {
  renderableDome = new RenderableDome(50, 20);
  renderableArcs = new RenderableWormArcs(gameConfig->maximumPlayers, 4);
  renderableHeads = new RenderableWormHeads(gameConfig->maximumPlayers, gameConfig->wormWidth);

  domeGrid = renderer->addRenderable(renderableDome, GL_LINES, "domeShader.vert", "domeGridShader.frag", true);
  domeWorms = renderer->addRenderable(renderableDome, GL_TRIANGLES, "domeShader.vert", "domeWormsShader.frag", true);
  collision = renderer->addRenderable(renderableDome, GL_TRIANGLES, "domeShader.vert", "collisionShader.frag", true);

  renderableArcs->setWormArcs(wormArcs->getVal());
  wormLines = renderer->addRenderable(renderableArcs, GL_TRIANGLES, "wormShader.vert", "wormShader.frag", false);
  renderableHeads->setWormHeads(wormHeads->getVal());
  wormDots = renderer->addRenderable(renderableHeads, GL_TRIANGLES, "wormHeadShader.vert", "wormHeadShader.frag", false);

  timer.setVal(0);

  timeUni = new Uniform<float>("time");
  renderer->setUniform(wormDots, timeUni);
  collisionsUni = new Uniform<std::vector<glm::vec4> >("collisions");
  renderer->setUniform(collision, collisionsUni);
  collisionCountUni = new Uniform<GLuint>("collisionCount");
  renderer->setUniform(collision, collisionCountUni);

  attached = true;
}

void GameClusterState::reset() {
  renderer->resetFBO(wormLines);
}

void GameClusterState::detach() {
  renderer->removeRenderable(domeGrid);
  renderer->removeRenderable(domeWorms);
  renderer->removeRenderable(collision);
  renderer->removeRenderable(wormLines);
  renderer->removeRenderable(wormDots);
  delete renderableDome;

  renderableDome = nullptr;
  delete renderableArcs;
  renderableArcs = nullptr;
  delete renderableHeads;
  renderableHeads = nullptr;

  delete timeUni;
  timeUni = nullptr;
  delete collisionsUni;
  timeUni = nullptr;

  attached = false;
}

void GameClusterState::preSync() {
  if (renderSpace != nullptr) {
    std::vector<WormArc> arcs = renderSpace->getArcs();
    std::vector<WormHead> heads = renderSpace->getHeads();
    std::vector<WormCollision> collisions = renderSpace->getCollisions();
    bool reset = renderSpace->getResetSignal();

    wormArcs->setVal(arcs);
    wormCollisions->setVal(collisions);
    wormHeads->setVal(heads);
    timer.setVal(timer.getVal() + 1);

    resetSignal->setVal(reset);
    renderSpace->clear();
  }

  // Reset stitch step
  stitchStep = 0;

  if (resetSignal->getVal()) {
    reset();
  }
}

void GameClusterState::postSyncPreDraw() {
  // Copy current worm positions and colors
  std::vector<WormArc> arcs = wormArcs->getVal();
  std::vector<WormHead> heads = wormHeads->getVal();
  std::vector<WormCollision> collisions = wormCollisions->getVal();

  renderableArcs->setWormArcs(arcs);
  renderableHeads->setWormHeads(heads);

  int tickNumber = timer.getVal();
  timeUni->set(tickNumber);

  for (auto c : collisions) {
    collisionTimerQueue.push_back({tickNumber, c});
  }

  while ( !collisionTimerQueue.empty() && (tickNumber - collisionTimerQueue.front().first) >= COLLISION_DURATION) {
    collisionTimerQueue.pop_front();
  }

  std::vector<glm::vec4> activeCollisions;
  for (auto pair : collisionTimerQueue) {
    glm::vec3 pos = (glm::vec3) pair.second.getCartesianPosition();
    float lifeTime = 1.0 - (float)(tickNumber - pair.first)/(float)COLLISION_DURATION;
    activeCollisions.push_back(glm::vec4(pos.x, pos.y, pos.z, lifeTime));
  }
  collisionsUni->set(activeCollisions);
  collisionCountUni->set((GLuint)activeCollisions.size());
}

void GameClusterState::draw() {

  renderer->render(wormDots);

  // if (!heads.empty() && heads.at(0).isInGap()) {
  //   renderer->render(wormDots);
  // } else {
  //   renderer->renderToFBO(wormDots, stitchStep);
  // }

  // render wormLines to FBO
  renderer->renderToFBO(wormLines, stitchStep);
  // render grid lines
  renderer->render(domeGrid);
  // render FBO as texture on dome
  renderer->render(domeWorms, wormLines, stitchStep);
  // renderer->render(domeWorms, wormDots, stitchStep);

  renderer->render(collision);

  ++stitchStep;
}

void GameClusterState::encode() {
  // get things from renderSpace and send it to everyone.
  sgct::SharedData *data = sgct::SharedData::instance();

  data->writeBool(resetSignal);
  data->writeVector(wormArcs);
  data->writeVector(wormCollisions);
  data->writeVector(wormHeads);
  data->writeInt(&timer);
}

void GameClusterState::decode() {
  // read from buffer and insert data to GameRenderers.
  sgct::SharedData *data = sgct::SharedData::instance();

  data->writeBool(resetSignal);
  data->readVector(wormArcs);
  data->readVector(wormCollisions);
  data->readVector(wormHeads);
  data->readInt(&timer);
}
