#include "sgct.h"
#include <iostream>
#include <gameConfig.h>
#include <Webserver.h>
#include <fisheyeCollisionSpace.h>
#include <clusterRenderSpace.h>
#include <wormTracker.h>
#include <jsonActionResolver.h>
#include <gameEngine.h>
#include <gameController.h>
#include <playerGameController.h>
#include <adminGameController.h>
#include <keyboardGameController.h>
#include <rainbowColorTheme.h>
#include <uniformDistributor.h>
#include <dumbAIController.h>

// Render states
#include <clusterState.h>
#include <gameClusterState.h>
#include <lobbyClusterState.h>
#include <syncMaster.h>
#include <tweener.h>

sgct::Engine * gEngine;

void myDrawFun();
void myPreSyncFun();
void myInitOGLFun();
void myEncodeFun();
void myDecodeFun();
void myCleanUpFun();
void myPostSyncPreDrawFun();

void keyCallback(int key, int action);
void mouseButtonCallback(int button, int action);

// Game Engine
GameEngine *gameEngine;
ClusterRenderSpace *renderSpace;
std::vector<GameController*> gameControllers;
KeyboardGameController *keyboardGameController;
DumbAIController *dumbAIController;
SyncMaster *syncMaster;
GameConfig *gameConfig;
LobbyClusterState *lcs;
GameClusterState *gcs;
FisheyeCollisionSpace *fisheyeSpace;
UniformDistributor *distributor;
ColorTheme *ct;
WormTracker *wt;
PlayerManager *pm;
Webserver *webServer;
JsonActionResolver *actionResolver;
JsonBuilder *dataSerializationBuilder;
PlayerGameController *pgc;
AdminGameController* agc;

  int main( int argc, char* argv[] ) {
  // BAE, load the AchDome fonts!
  sgct_text::FontManager::instance()->addFont("Comfortaa", "fonts/Comfortaa-Regular.ttf", sgct_text::FontManager::FontPath_Local);
  sgct_text::FontManager::instance()->addFont("Comfortaa-Bold", "fonts/Comfortaa-Bold.ttf", sgct_text::FontManager::FontPath_Local);
  sgct_text::FontManager::instance()->addFont("Comfortaa-Light", "fonts/Comfortaa-Light.ttf", sgct_text::FontManager::FontPath_Local);

  gEngine = new sgct::Engine( argc, argv );

  gEngine->setInitOGLFunction( myInitOGLFun );
  gEngine->setDrawFunction( myDrawFun );
  gEngine->setPreSyncFunction( myPreSyncFun );
  gEngine->setPostSyncPreDrawFunction ( myPostSyncPreDrawFun );
  gEngine->setKeyboardCallbackFunction( keyCallback );
  gEngine->setMouseButtonCallbackFunction( mouseButtonCallback );
  gEngine->setCleanUpFunction( myCleanUpFun );

  sgct::SharedData::instance()->setEncodeFunction( myEncodeFun );
  sgct::SharedData::instance()->setDecodeFunction( myDecodeFun );
  gEngine->setClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  syncMaster = new SyncMaster();
  gameConfig = new GameConfig("gameTest.json");

  if (!gEngine->init( sgct::Engine::OpenGL_3_3_Core_Profile )) {
      delete gEngine;
      return EXIT_FAILURE;
  }

  if (gEngine->isMaster()){

    fisheyeSpace = new FisheyeCollisionSpace(2000);
    renderSpace = new ClusterRenderSpace();
    distributor = new UniformDistributor(gameConfig);

    ct = new RainbowColorTheme();

    wt = new WormTracker(fisheyeSpace, renderSpace, distributor, gameConfig);
    pm = new PlayerManager(ct);

    gameEngine = new GameEngine(wt, pm, gameConfig);

    webServer = new Webserver();
    webServer->start(8000);

    actionResolver = new JsonActionResolver();
    dataSerializationBuilder = new JsonBuilder();

    pgc = new PlayerGameController(gameEngine, webServer, actionResolver, dataSerializationBuilder, "player");
    agc = new AdminGameController(gameEngine, webServer, actionResolver, dataSerializationBuilder, "admin", gameConfig);
    gameControllers.push_back(pgc);
    gameControllers.push_back(agc);

    // ics = new IntroClusterState();
    lcs = new LobbyClusterState(gEngine, gameConfig, pm);
    gcs = new GameClusterState(gEngine, gameConfig, renderSpace, gameEngine, wt);

    keyboardGameController = new KeyboardGameController(gameEngine);
    if(gameConfig->numAiPlayers > 0){
      dumbAIController = new DumbAIController(gameEngine, gameConfig->numAiPlayers);
      gameControllers.push_back(dumbAIController);
    }

    gameControllers.push_back(keyboardGameController);
  } else {
    // isSlave:
    // ics = new IntroClusterState();
    gameEngine = nullptr;
    lcs = new LobbyClusterState(gEngine, gameConfig);
    gcs = new GameClusterState(gEngine, gameConfig);
  }

  std::map<GameEngine::State, ClusterState*> stateMap;
  stateMap[GameEngine::LOBBY] = lcs;
  stateMap[GameEngine::COUNTDOWN] = gcs;
  stateMap[GameEngine::GAME] = gcs;
  stateMap[GameEngine::GAME_OVER] = gcs;
  syncMaster = new SyncMaster(gameEngine, stateMap);

  syncMaster->setGameEngine(gameEngine);
  syncMaster->setStateMap(stateMap);
  syncMaster->setInitDone();


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
  }
  Tweener::getInstance()->tick();
  syncMaster->preSync();
  gEngine->checkForOGLErrors();
}

void myPostSyncPreDrawFun() {
  syncMaster->postSyncPreDraw();
}

void myDrawFun() {
  syncMaster->draw();
}

void myEncodeFun() {
  gameConfig->encode();
  syncMaster->encode();
}

void myDecodeFun() {
  gameConfig->decode();
  syncMaster->decode();
}

void keyCallback(int key, int action) {
  if (gEngine->isMaster()){
    keyboardGameController->processKeyEvent(key, action);
  }
}

void mouseButtonCallback(int button, int action) {
}

void myCleanUpFun() {
  if(dumbAIController){
    delete dumbAIController;
  }
  delete gameEngine;
  delete renderSpace;
  delete keyboardGameController;
  delete syncMaster;
  delete gameConfig;
  delete lcs;
  delete gcs;
  delete fisheyeSpace;
  delete distributor;
  delete ct;
  delete wt;
  delete pm;
  delete webServer;
  delete actionResolver;
  delete dataSerializationBuilder;
  delete pgc;
  delete agc;
}
