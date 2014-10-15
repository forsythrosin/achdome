#include "sgct.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shaderUtils.h>
#include <wormHead.h>
#include <websocketBufferQueue.h>
#include <Webserver.h>
#include <fisheyeCollisionSpace.h>
#include <clusterRenderSpace.h>
#include <wormTracker.h>
#include <domeSurface.h>
#include <jsonActionResolver.h>
#include <gameEngine.h>
#include <gameController.h>
#include <socketGameController.h>
#include <keyboardGameController.h>

sgct::Engine * gEngine;
Webserver webserver;
WebsocketBufferQueue bufferQueue(&webserver);
JsonActionResolver *actionResolver;

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

// DomeSurface buffers
GLuint VBO = GL_FALSE;
GLuint IBO = GL_FALSE;
GLint Matrix_Loc = -1;
GLuint vertexArray;
DomeSurface *domeSurface;
KeyboardGameController *keyboardGameController;

int main( int argc, char* argv[] ) {
  gEngine = new sgct::Engine( argc, argv );
  actionResolver = new JsonActionResolver();

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
    gameEngine = new GameEngine(wt);

    SocketGameController *sgc = new SocketGameController(gameEngine, 8000);
    gameControllers.push_back(sgc);

    keyboardGameController = new KeyboardGameController(gameEngine);
    gameControllers.push_back(keyboardGameController);
  }

  // Main loop
  gEngine->render();

  // Clean up
  delete gEngine;
  delete actionResolver;

  // Exit program
  exit( EXIT_SUCCESS );
}

void myInitOGLFun() {  
  domeSurface = new DomeSurface(100, 30);

  const GLfloat* domeVertexData = domeSurface->getSphericalVertexData();
  const GLuint* domeTriangleData = domeSurface->getTriangleData();

  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  // vertex buffer
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, domeSurface->getVertexCount()*sizeof(GLfloat)*2, domeVertexData, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  // index buffer
  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, domeSurface->getTriangleCount()*sizeof(GLuint)*3, domeTriangleData, GL_STATIC_DRAW);

  glBindVertexArray(0); //unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  std::string vertShader = shaderUtils::absolutePathToShader("domeShader.vert");
  std::string fragShader = shaderUtils::absolutePathToShader("domeShader.frag");
  sgct::ShaderManager::instance()->addShaderProgram( "xform", vertShader, fragShader );
  sgct::ShaderManager::instance()->bindShaderProgram( "xform" );
  Matrix_Loc = sgct::ShaderManager::instance()->getShaderProgram( "xform" ).getUniformLocation( "MVP" );

  sgct::ShaderManager::instance()->unBindShaderProgram();
}

void myPreSyncFun() {
  if(gEngine->isMaster()){
    for (GameController *ge : gameControllers) {
      ge->performActions();
    }
    gameEngine->tick();
  }
}

void myDrawFun() {
  glm::mat4 scene_mat = glm::rotate( glm::mat4(1.0f), -117.0f, glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 MVP = gEngine->getActiveModelViewProjectionMatrix() * scene_mat;

  sgct::ShaderManager::instance()->bindShaderProgram( "xform" );

  glUniformMatrix4fv(Matrix_Loc, 1, GL_FALSE, &MVP[0][0]);

  glBindVertexArray(vertexArray);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glDrawElements(GL_LINE_LOOP, domeSurface->getTriangleCount()*3, GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);

  //unbind
  glBindVertexArray(0);
  sgct::ShaderManager::instance()->unBindShaderProgram();
}

void myEncodeFun() {
  // get things from renderSpace and send it to everyone.
}

void myDecodeFun() {
  // read from buffer and insert data to GameRenderers.
}

void keyCallback(int key, int action) {
  keyboardGameController->processKeyEvent(key, action);
}

void mouseButtonCallback(int button, int action) {
}

void myCleanUpFun() {
  if (VBO)
    glDeleteBuffers(1, &VBO);
  if (IBO)
    glDeleteBuffers(1, &IBO);
  if (vertexArray)
    glDeleteVertexArrays(1, &vertexArray);
}
