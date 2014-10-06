#include "sgct.h"
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <shaderUtils.h>
#include <wormHead.h>
#include <domeSurface.h>

#include <Webserver.h>
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

// DomeSurface buffers
GLuint VBO = GL_FALSE;
GLuint IBO = GL_FALSE;
GLint Matrix_Loc = -1;
GLuint vertexArray;
DomeSurface *domeSurface;

int main( int argc, char* argv[] ) {

  Webserver webserver;
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

  if(gEngine->isMaster()){
    webserver.setCallback(webDecoder);
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

  domeSurface = new DomeSurface(100, 30);
  // const GLfloat* domeVertexData = domeSurface->getCartesianVertexData();
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
  glDrawElements(GL_LINES, domeSurface->getTriangleCount()*3, GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);

  //unbind
  glBindVertexArray(0);
  sgct::ShaderManager::instance()->unBindShaderProgram();
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
  // if(vertexPositionBuffer)
  //   glDeleteBuffers(1, &vertexPositionBuffer);
  // if(vertexArray)
  //   glDeleteVertexArrays(1, &vertexArray);
}

 void webDecoder(const char *msg, size_t len){
     Webserver::instance()->addBroadcast(msg);
     std::cout << "Decoded message from client: " << msg << std::endl;
 }
