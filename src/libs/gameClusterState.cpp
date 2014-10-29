#include <gameClusterState.h>

#include <renderable.h>
#include <renderer.h>
#include <renderableDome.h>
#include <renderableWormGroup.h>
#include <clusterRenderSpace.h>
#include <iostream>

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig) : GameClusterState(gEngine, gameConfig, nullptr) {}

GameClusterState::GameClusterState(sgct::Engine *gEngine, GameConfig *gameConfig, ClusterRenderSpace *rs) : ClusterState(gEngine, gameConfig) {
  wormArcs = new sgct::SharedVector<WormArc>(2);
  wormArcs = new sgct::SharedVector<WormArc>(100);
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
    //    std::vector<WormCollision> collisions = renderSpace->getCollisions();

    glm::quat first(glm::vec3(0.0, -0.5, 2.0*glm::pi<float>()));
    glm::quat second(glm::vec3(0.0, -0.5, 2.0*glm::pi<float>() + 0.000001));
    glm::vec4 c(1.0, 1.0, 0.0, 1.0);
    WormArc wa(0, first, second, 0, c);
    if (arcs.size() < 1) {
      arcs.push_back(wa);
    }

    wormArcs->setVal(arcs);

    //    std::cout << arcs.size() << std::endl;
    //  std::vector<WormHeadIndicator> heads = renderSpace->getWormHeadIndicators();
    //  wormHeadIndicators->setVal(heads);
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
  sgct::SharedData::instance()->writeVector(wormArcs);
}
void GameClusterState::decode() {
  // read from buffer and insert data to GameRenderers.
  sgct::SharedData::instance()->readVector(wormArcs);
}
