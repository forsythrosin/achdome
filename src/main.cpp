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
#include <jsonBuilder.h>
#include <gameEngine.h>
#include <gameController.h>
#include <socketGameController.h>
#include <keyboardGameController.h>
#include <rainbowColorTheme.h>
#include <renderableDome.h>
#include <renderableWormGroup.h>
#include <Webserver.h>
#include <wormArc.h>
#include <renderer.h>
#include <cmath>

// Render states
#include <renderState.h>
#include <gameRenderState.h>

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

RenderState *renderState;

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
  gEngine->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  if (!gEngine->init( sgct::Engine::OpenGL_3_3_Core_Profile )) {
      delete gEngine;
      return EXIT_FAILURE;
  }

  if (gEngine->isMaster()){
    FisheyeCollisionSpace *fisheyeSpace = new FisheyeCollisionSpace(100);
    renderSpace = new ClusterRenderSpace();

    ColorTheme *ct = new RainbowColorTheme();

    WormTracker* wt = new WormTracker(fisheyeSpace, renderSpace);
    PlayerManager *pm = new PlayerManager(ct);
    gameEngine = new GameEngine(wt, pm);

    Webserver *webServer = new Webserver();
    webServer->start(8000);

    JsonActionResolver *actionResolver = new JsonActionResolver();
    JsonBuilder *dataSerializationBuilder = new JsonBuilder();

    SocketGameController *sgc = new SocketGameController(gameEngine, webServer, actionResolver, dataSerializationBuilder);
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
  renderState = new GameRenderState(gEngine);
  renderState->init();
}

void myPreSyncFun() {
  if(gEngine->isMaster()){
    for (GameController *ge : gameControllers) {
      ge->performActions();
    }
    gameEngine->tick();
  }
  renderState->preSync();
}

void myDrawFun() {
  renderState->draw();
}

void myEncodeFun() {
  renderState->encode();
}

void myDecodeFun() {
  renderState->decode();
}

void keyCallback(int key, int action) {
  if (gEngine->isMaster()){
    keyboardGameController->processKeyEvent(key, action);
  }
}

void mouseButtonCallback(int button, int action) {
}

void myCleanUpFun() {
}
