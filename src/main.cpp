#include "sgct.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/constants.hpp>

// #include <shaderUtils.h>
#include <wormHead.h>
#include <Webserver.h>
#include <fisheyeCollisionSpace.h>
#include <clusterRenderSpace.h>
#include <wormTracker.h>
#include <jsonActionResolver.h>
#include <gameEngine.h>
#include <gameController.h>
#include <socketGameController.h>
#include <keyboardGameController.h>
#include <renderableDome.h>
#include <renderableWormGroup.h>
#include <Webserver.h>
#include <wormArc.h>
#include <renderer.h>
#include <cmath>

sgct::Engine * gEngine;

void myDrawFun();
void myPreSyncFun();
void myInitOGLFun();
void myEncodeFun();
void myDecodeFun();
void myCleanUpFun();

void webDecoder(const char *, size_t);

void keyCallback(int key, int action);
void mouseButtonCallback(int button, int action);

// Game Engine
GameEngine *gameEngine;
ClusterRenderSpace *renderSpace;
std::vector<GameController*> gameControllers;
KeyboardGameController *keyboardGameController;

// Renderer + renderables
sgct::SharedVector<WormArc> wormArcs(1);
Renderer *renderer;
RenderableDome *dome;
RenderableWormGroup *worms;
int wormsId;
float timer = 0.0f;

int main( int argc, char* argv[] ) {
  gEngine = new sgct::Engine( argc, argv );

  gEngine->setInitOGLFunction( myInitOGLFun );
  gEngine->setDrawFunction( myDrawFun );
  gEngine->setPreSyncFunction( myPreSyncFun );
  gEngine->setKeyboardCallbackFunction( keyCallback );
  gEngine->setMouseButtonCallbackFunction( mouseButtonCallback );
  gEngine->setCleanUpFunction( myCleanUpFun );
  sgct::SharedData::instance()->setEncodeFunction( myEncodeFun );
  sgct::SharedData::instance()->setDecodeFunction( myDecodeFun );
  gEngine->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  if (!gEngine->init( sgct::Engine::OpenGL_3_3_Core_Profile )) {
      delete gEngine;
      return EXIT_FAILURE;
  }

  if (gEngine->isMaster()){
    FisheyeCollisionSpace *fisheyeSpace = new FisheyeCollisionSpace(100);
    renderSpace = new ClusterRenderSpace();

    WormTracker* wt = new WormTracker(fisheyeSpace, renderSpace);
    PlayerManager *pm = new PlayerManager();
    gameEngine = new GameEngine(wt, pm);

    Webserver *webServer = new Webserver();
    webServer->start(8000);

    JsonActionResolver *actionResolver = new JsonActionResolver();

    SocketGameController *sgc = new SocketGameController(gameEngine, webServer, actionResolver);
    gameControllers.push_back(sgc);

    keyboardGameController = new KeyboardGameController(gameEngine);
    gameControllers.push_back(keyboardGameController);
  }

  // Main loop
  gEngine->render();

  // Clean up
  delete gEngine;

  // Exit program
  exit( EXIT_SUCCESS );
}

void myInitOGLFun() {
  renderer = new Renderer(gEngine);

  dome = new RenderableDome(50, 20);
  renderer->addRenderable(dome, GL_LINES, "domeShader.vert", "domeShader.frag", true);

  worms = new RenderableWormGroup(1, 20);
  worms->setWormArcs(wormArcs.getVal());
  wormsId = renderer->addRenderable(worms, GL_LINES, "wormShader.vert", "wormShader.frag", false);
}

void myPreSyncFun() {
  if(gEngine->isMaster()){
    for (GameController *ge : gameControllers) {
      ge->performActions();
    }
    gameEngine->tick();
  }

  // Update worm positions
  if( gEngine->isMaster() ) {
    glm::quat first(glm::vec3(0.0, -0.5, 2.0*glm::pi<float>()*timer));
    glm::quat second(glm::vec3(0.0, -0.5, 2.0*glm::pi<float>()*timer + 1.0));

    timer += 0.005f;
    WormArc wa(0, first, second);

    std::vector<WormArc> arcs;
    arcs.push_back(wa);
    wormArcs.setVal(arcs);
  }
}

void myDrawFun() {
  worms->setWormArcs(wormArcs.getVal());
  renderer->render(wormsId);
}

void myEncodeFun() {
  // get things from renderSpace and send it to everyone.
  sgct::SharedData::instance()->writeVector( &wormArcs );
}

void myDecodeFun() {
  // read from buffer and insert data to GameRenderers.
  sgct::SharedData::instance()->readVector( &wormArcs );
}

void keyCallback(int key, int action) {
  if (gEngine->isMaster()){
    keyboardGameController->processKeyEvent(key, action);
  }
}

void mouseButtonCallback(int button, int action) {
}

void myCleanUpFun() {
  delete dome;
  delete worms;
  delete renderer;
}
