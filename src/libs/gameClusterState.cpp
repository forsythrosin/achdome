#include <gameClusterState.h>
#include <renderableDome.h>
#include <renderableWormArcs.h>
#include <renderableWormHeads.h>
#include <clusterRenderSpace.h>
#include <gameEngine.h>
#include <wormArc.h>
#include <wormArcSyncData.h>
#include <wormCollision.h>
#include <wormCollisionSyncData.h>
#include <wormHead.h>
#include <wormHeadSyncData.h>
#include <wormHeadAppearance.h>
#include <wormTracker.h>
#include <cmath>


GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : GameClusterState(gEngine, gameConfig, nullptr, nullptr, nullptr) {}

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs, GameEngine *ge, WormTracker *wt) : ClusterState(gEngine, gameConfig) {
  resetSignal = new sgct::SharedBool(false);
  wormArcs = new sgct::SharedVector<WormArcSyncData>(gameConfig->maximumPlayers);
  wormCollisions = new sgct::SharedVector<WormCollisionSyncData>(gameConfig->maximumPlayers);
  wormHeads = new sgct::SharedVector<WormHeadSyncData>(gameConfig->maximumPlayers);

  renderableDome = nullptr;
  renderableArcs = nullptr;
  renderableHeads = nullptr;

  timeUni = nullptr;
  collisionsUni = nullptr;

  renderSpace = rs;
  gameEngine = ge;
  wormTracker = wt;

  if (wormTracker != nullptr) {
    wormTracker->addEventListener(this);
  }
  attached = false;
}

GameClusterState::~GameClusterState() {
  if (renderableDome != nullptr) delete renderableDome;
  if (renderableArcs != nullptr) delete renderableArcs;
  if (renderableHeads != nullptr) delete renderableHeads;

  if (wormArcs != nullptr) delete wormArcs;
  if (wormCollisions != nullptr) delete wormCollisions;
  if (wormHeads != nullptr) delete wormHeads;

  if (collisionsUni != nullptr) delete collisionsUni;

  if (wormTracker != nullptr) {
    wormTracker->removeEventListener(this);
  }
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

  gridColorUni = new Uniform<glm::vec4>("gridColor");
  timeUni = new Uniform<float>("gameTime");
  renderer->setUniform(domeGrid, gridColorUni);

  collisionsUni = new Uniform<std::vector<glm::vec4> >("collisions");
  renderer->setUniform(collision, collisionsUni);
  collisionCountUni = new Uniform<GLuint>("collisionCount");
  renderer->setUniform(collision, collisionCountUni);
  collisionColorsUni = new Uniform<std::vector<glm::vec4> >("collisionColors");
  renderer->setUniform(collision, collisionColorsUni);

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

    std::vector<WormArcSyncData> syncArcs;
    for (WormArc arc : arcs) {
      glm::vec4 color = gameEngine->getColor(arc.getWormId());
      syncArcs.push_back(WormArcSyncData(arc, color));
    }

    std::vector<WormCollisionSyncData> syncCollisions;
    for (WormCollision collision : collisions) {
      glm::vec4 color = gameEngine->getColor(collision.collider);
      syncCollisions.push_back(WormCollisionSyncData(collision, color));
    }

    std::vector<WormHeadSyncData> syncHeads;
    for (WormHead head : heads) {
      WormHeadAppearance *headAppearance = getWormHeadAppearance(head);
      syncHeads.push_back(WormHeadSyncData(head, *headAppearance));
    }

    wormArcs->setVal(syncArcs);
    wormCollisions->setVal(syncCollisions);
    wormHeads->setVal(syncHeads);

    countdownSecondsLeft.setVal(gameEngine->getSecondsLeftInCountdown());
    gameSecondsPassed.setVal(gameEngine->getSecondsSinceGameStarted());

    resetSignal->setVal(reset);
    renderSpace->clear();
  }

  // Reset stitch step
  stitchStep = 0;

  if (resetSignal->getVal()) {
    reset();
  }
}

WormHeadAppearance* GameClusterState::getWormHeadAppearance(WormHead head) {
  int id = head.getWormId();
  glm::vec4 color = gameEngine->getColor(id);

  WormHeadAppearance *headAppearance;
  if (wormHeadAppearances.count(id) == 0) {
    wormHeadAppearances[id] = new WormHeadAppearance(color, 0, 0, 0, 0);
  }
  return wormHeadAppearances[id];
}

void GameClusterState::postSyncPreDraw() {
  // Copy current worm positions and colors
  std::vector<WormArcSyncData> syncArcs = wormArcs->getVal();
  std::vector<WormHeadSyncData> syncHeads = wormHeads->getVal();
  std::vector<WormCollisionSyncData> syncCollisions = wormCollisions->getVal();
  float countdown = countdownSecondsLeft.getVal();
  float gameTime = gameSecondsPassed.getVal();

  renderableArcs->setWormArcs(syncArcs);
  renderableHeads->setWormHeads(syncHeads);

  timeUni->set(gameTime);

  if (gameTime >= 1) {
    glm::vec4 gray(1.0, 1.0, 1.0, 0.1*std::min(1.0f, std::max(gameTime - 1.0f, 0.0f)));
    gridColorUni->set(gray);
  } else if (gameTime >= 0) {
    glm::vec4 green(0.0, 1.0, 0.0, 1.0 - std::min(1.0f, gameTime));
    gridColorUni->set(green);
  } else if (countdown <= 1) {
    gridColorUni->set(glm::vec4(1.0, 1.0, 0.0, 0.6 * fmod(countdown, 1.0)));
  } else {
    gridColorUni->set(glm::vec4(1.0, 0.0, 0.0, 0.6 * fmod(countdown, 1.0)));
  }

  for (auto c : syncCollisions) {
    collisionTimerQueue.push_back({gameTime, c});
  }

  while (!collisionTimerQueue.empty() && (gameTime - collisionTimerQueue.front().first) >= COLLISION_DURATION) {
    collisionTimerQueue.pop_front();
  }

  std::vector<glm::vec4> activeCollisions;
  std::vector<glm::vec4> collisionColors;
  for (auto pair : collisionTimerQueue) {
    glm::vec3 pos = (glm::vec3) pair.second.getPosition();
    glm::vec4 color = pair.second.getColor();
    float lifeTime = 1.0 - (float)(gameTime - pair.first)/COLLISION_DURATION;
    activeCollisions.push_back(glm::vec4(pos.x, pos.y, pos.z, lifeTime));
    collisionColors.push_back(color);
  }
  collisionsUni->set(activeCollisions);
  collisionColorsUni->set(collisionColors);
  collisionCountUni->set((GLuint)activeCollisions.size());
}

void GameClusterState::draw() {
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

  renderer->render(wormDots);
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

  data->writeFloat(&countdownSecondsLeft);
  data->writeFloat(&gameSecondsPassed);
}

void GameClusterState::decode() {
  // read from buffer and insert data to GameRenderers.
  sgct::SharedData *data = sgct::SharedData::instance();

  data->writeBool(resetSignal);
  data->readVector(wormArcs);
  data->readVector(wormCollisions);
  data->readVector(wormHeads);

  data->readFloat(&countdownSecondsLeft);
  data->readFloat(&gameSecondsPassed);

}

void GameClusterState::onWormStarted(WormHead head) {
  WormHeadAppearance *headAppearance = getWormHeadAppearance(head);
  headAppearance->tweenDiameterCoefficient(0.0, 0.5);
  headAppearance->tweenStrokeWidth(0.005, 0.2);
  headAppearance->tweenArrowLength(0.0, 0.5);
  headAppearance->tweenArrowWidth(0.0, 0.2);
}


void GameClusterState::onWormSpawned(WormHead head) {
  WormHeadAppearance *headAppearance = getWormHeadAppearance(head);
  headAppearance->setDiameterCoefficient(0.0);
  headAppearance->setStrokeWidth(0.0);
  headAppearance->setArrowLength(0.0);
  headAppearance->setArrowWidth(0.0);


  headAppearance->tweenDiameterCoefficient(5.0, 0.3);
  headAppearance->tweenStrokeWidth(0.005, 0.3);
  headAppearance->tweenArrowLength(0.06, 0.3);
  headAppearance->tweenArrowWidth(0.03, 0.3);

}
