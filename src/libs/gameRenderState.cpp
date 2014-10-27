#include <gameRenderState.h>

#include <renderable.h>
#include <renderer.h>
#include <renderableDome.h>
#include <renderableWormGroup.h>

GameRenderState::GameRenderState(sgct::Engine *gEngine) : RenderState(gEngine) {
  wormArcs = new sgct::SharedVector<WormArc>(100);
}

GameRenderState::~GameRenderState() {
  delete dome;
  delete worms;
  delete wormArcs;
}

void GameRenderState::init() {
  dome = new RenderableDome(50, 20);
  domeWorms = renderer->addRenderable(dome, GL_TRIANGLES, "domeShader.vert", "domeWormsShader.frag", true);
  domeGrid = renderer->addRenderable(dome, GL_LINES, "domeShader.vert", "domeGridShader.frag", true);

  worms = new RenderableWormGroup(100, 4, 0.05);
  worms->setWormArcs(wormArcs->getVal());

  glm::vec4 red(1.0, 0.0, 0.0, 1.0);
  glm::vec4 blue(0.0, 0.0, 1.0, 1.0);
  std::vector<glm::vec4> colors = {red, blue};

  worms->setWormColors(colors);
  wormLines = renderer->addRenderable(worms, GL_TRIANGLES, "wormShader.vert", "wormShader.frag", false);
}

void GameRenderState::preSync() {
  // Update worm positions
  if( gEngine->isMaster() ) {
    glm::quat first0(glm::vec3(0.0, -0.785375, 2.0*glm::pi<float>()*timer));
    glm::quat second0(glm::vec3(0.0, -0.785375, 2.0*glm::pi<float>()*timer + 0.1));

    glm::quat first1(glm::vec3(0.0, -1.0, 2.0*glm::pi<float>()*timer));
    glm::quat second1(glm::vec3(0.0, -1.0, 2.0*glm::pi<float>()*timer + 0.1));

    timer += 0.005f;
    WormArc wa0(0, first0, second0);
    WormArc wa1(0, first1, second1);

    std::vector<WormArc> arcs;
    if (arcs.size() < 1) {
      arcs.push_back(wa0);
      arcs.push_back(wa1);
    }
    wormArcs->setVal(arcs);

    // renderSpace->clear();
  }

  // Reset stitch step
  stitchStep = 0;
}

void GameRenderState::draw() {
  // Copy current worm positions and colors
  worms->setWormArcs(wormArcs->getVal());

  // render wormLines to FBO
  renderer->renderToFBO(wormLines, stitchStep);
  // render FBO as texture on dome
  renderer->render(domeWorms, wormLines, stitchStep);
  // render grid lines
  renderer->render(domeGrid);

  ++stitchStep;
}

void GameRenderState::encode() {
  // get things from renderSpace and send it to everyone.
  sgct::SharedData::instance()->writeVector(wormArcs);
}
void GameRenderState::decode() {
  // read from buffer and insert data to GameRenderers.
  sgct::SharedData::instance()->readVector(wormArcs);
}
