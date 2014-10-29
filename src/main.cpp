#include "sgct.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/constants.hpp>
#include <gameConfig.h>
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
#include <uniformDistributor.h>
#include <renderableDome.h>
#include <renderableWormGroup.h>
#include <Webserver.h>
#include <wormArc.h>
#include <renderer.h>
#include <cmath>

// Render states
#include <clusterState.h>
#include <gameClusterState.h>
#include <lobbyClusterState.h>
#include <syncMaster.h>

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
SyncMaster *syncMaster;

//IntroClusterState *ics;
LobbyClusterState *lcs;
GameClusterState *gcs;

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

  GameConfig *gameConfig = new GameConfig("gameTest.json");
  if (gEngine->isMaster()){

    FisheyeCollisionSpace *fisheyeSpace = new FisheyeCollisionSpace(500);
    renderSpace = new ClusterRenderSpace();
    UniformDistributor *distributor = new UniformDistributor();

    ColorTheme *ct = new RainbowColorTheme();

    WormTracker* wt = new WormTracker(fisheyeSpace, renderSpace, distributor);
    PlayerManager *pm = new PlayerManager(ct);

    gameEngine = new GameEngine(wt, pm, gameConfig);

    Webserver *webServer = new Webserver();
    webServer->start(8000);

    JsonActionResolver *actionResolver = new JsonActionResolver();
    JsonBuilder *dataSerializationBuilder = new JsonBuilder();

    SocketGameController *sgc = new SocketGameController(gameEngine, webServer, actionResolver, dataSerializationBuilder);
    gameControllers.push_back(sgc);

    // ics = new IntroClusterState();
    lcs = new LobbyClusterState(gEngine, gameConfig);
    gcs = new GameClusterState(gEngine, renderSpace, gameConfig);
    
    keyboardGameController = new KeyboardGameController(gameEngine);
    gameControllers.push_back(keyboardGameController);
  } else {
    // isSlave:
    // ics = new IntroClusterState();
    lcs = new LobbyClusterState(gEngine, gameConfig);
    gcs = new GameClusterState(gEngine, gameConfig);
  }

  std::map<GameEngine::State, ClusterState*> stateMap;
  //    stateMap[GameEngine::Intro] = ics;
  stateMap[GameEngine::LOBBY] = lcs;
  stateMap[GameEngine::GAME] = gcs;
  syncMaster = new SyncMaster(gameEngine, stateMap);

  // Main loop
  gEngine->render();

  // Clean up
  delete gEngine;

  // Exit program
  exit( EXIT_SUCCESS );
}


void myInitOGLFun() {
}

void myPreSyncFun() {
  if(gEngine->isMaster()){
    for (GameController *gc : gameControllers) {
      gc->performActions();
    }
    gameEngine->tick();
    syncMaster->preSync();
  }
}

void myDrawFun() {
  syncMaster->draw();
}

void myEncodeFun() {
  syncMaster->encode();
}

void myDecodeFun() {
  syncMaster->decode();
}

void keyCallback(int key, int action) {
  if (gEngine->isMaster()){
    keyboardGameController->processKeyEvent(key, action);
  }
  /*  
  delete renderState;
  renderState = new GameRenderState(gEngine);
  renderState->init();*/
}

void mouseButtonCallback(int button, int action) {
}

void myCleanUpFun() {
}
