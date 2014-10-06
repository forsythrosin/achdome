#include "sgct.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shaderUtils.h>
#include <wormHead.h>
#include <websocketBufferQueue.h>

#include "Webserver.h"
sgct::Engine * gEngine;
Webserver webserver;
WebsocketBufferQueue bufferQueue(&webserver);

void myDrawFun();
void myPreSyncFun();
void myInitOGLFun();
void myEncodeFun();
void myDecodeFun();
void myCleanUpFun();

void webDecoder(const char *, size_t);

void keyCallback(int key, int action);
void mouseButtonCallback(int button, int action);

int main( int argc, char* argv[] ) {
  gEngine = new sgct::Engine( argc, argv );
  
  gEngine->setInitOGLFunction( myInitOGLFun );
  gEngine->setDrawFunction( myDrawFun );
  gEngine->setPreSyncFunction( myPreSyncFun );
  gEngine->setKeyboardCallbackFunction( keyCallback );
  gEngine->setMouseButtonCallbackFunction( mouseButtonCallback );
  gEngine->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  gEngine->setCleanUpFunction( myCleanUpFun );
  sgct::SharedData::instance()->setEncodeFunction( myEncodeFun );
  sgct::SharedData::instance()->setDecodeFunction( myDecodeFun );
  
  if (!gEngine->init( sgct::Engine::OpenGL_3_3_Core_Profile )) {
      delete gEngine;
      return EXIT_FAILURE;
  }
  
  if(gEngine->isMaster()){
    webserver.start(8000);
  }
  // Main loop
  gEngine->render();

  // Clean up
  delete gEngine;

  // Exit program
  exit( EXIT_SUCCESS );
}

void myInitOGLFun() {
  WormHead wh(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.1, 0.0));
  
  std::cout << "Just testing some quaternions" << std::endl;
  for (int i = 0; i < 10; i++) {
    std::cout << wh.toString() << std::endl; 
    wh.move();
  }
}

void myPreSyncFun() {
    while(!bufferQueue.empty()){
        std::cout << bufferQueue.pop() << std::endl;
    }
}

void myDrawFun() {
}

void myEncodeFun() {
}

void myDecodeFun() {
}

void keyCallback(int key, int action) {
}

void mouseButtonCallback(int button, int action) {
}

void myCleanUpFun() {
}

 void webDecoder(const char *msg, size_t len){
     Webserver::instance()->addBroadcast(msg);
     std::cout << "Decoded message from client: " << msg << std::endl;
 }
