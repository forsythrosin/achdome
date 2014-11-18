#include <renderablePanel.h>
#include <iostream>
#include <stdio.h>

RenderablePanel::RenderablePanel() {
  vertexCount = VERTEX_COUNT;
  elementCount = ELEMENT_COUNT;

  sphericalVertexData.reserve(VERTEX_COUNT*2);
  textureCoordinates.reserve(VERTEX_COUNT*2);
  cartesianVertexData.reserve(VERTEX_COUNT*3);
  vertexColorData.reserve(VERTEX_COUNT*4);
  elementData.reserve(ELEMENT_COUNT*VERTS_PER_ELEMENT);

  createVertices();
  createElements();
};

RenderablePanel::~RenderablePanel() {};

void RenderablePanel::createVertices() {
  createVertices(glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0));
}

/**
 * Vertex placement
 */
void RenderablePanel::createVertices(glm::vec3 posTL, glm::vec3 posTR, glm::vec3 posBL, glm::vec3 posBR) {

  cartesianVertexData.clear();
  textureCoordinates.clear();

  // top left
  cartesianVertexData.push_back(posTL.x);
  cartesianVertexData.push_back(posTL.y);
  cartesianVertexData.push_back(posTL.z);

  textureCoordinates.push_back(0.0);
  textureCoordinates.push_back(0.0);

  // top right
  cartesianVertexData.push_back(posTR.x);
  cartesianVertexData.push_back(posTR.y);
  cartesianVertexData.push_back(posTR.z);

  textureCoordinates.push_back(1.0);
  textureCoordinates.push_back(0.0);

  // bottom left
  cartesianVertexData.push_back(posBL.x);
  cartesianVertexData.push_back(posBL.y);
  cartesianVertexData.push_back(posBL.z);

  textureCoordinates.push_back(0.0);
  textureCoordinates.push_back(1.0);

  // bottom right
  cartesianVertexData.push_back(posBR.x);
  cartesianVertexData.push_back(posBR.y);
  cartesianVertexData.push_back(posBR.z);

  textureCoordinates.push_back(1.0);
  textureCoordinates.push_back(1.0);
};

/**
 * Tessellation of the panel
 */
void RenderablePanel::createElements() {
  int topLeft = 0;
  int topRight = 1;
  int bottomLeft = 2;
  int bottomRight = 3;

  // top left face
  elementData.push_back(topLeft);
  elementData.push_back(bottomLeft);
  elementData.push_back(topRight);

  // bottom right face
  elementData.push_back(topRight);
  elementData.push_back(bottomLeft);
  elementData.push_back(bottomRight);
};

void RenderablePanel::setColor (glm::vec4 color) {
  int colorDim = 4;
  vertexColorData.clear();
  for (int j = 0; j < VERTEX_COUNT; ++j) {
    for (int i = 0; i < colorDim; ++i) {
      vertexColorData.push_back(color[i]);
    }
  }
};

void RenderablePanel::attach(){
  Renderable::attach();
  glGenBuffers(1, &textureCoordBuffer);
}

void RenderablePanel::detach(){
  Renderable::attach();
  glDeleteBuffers(1, &textureCoordBuffer);
}

void RenderablePanel::enableAttributes(){
  Renderable::enableAttributes();
  glEnableVertexAttribArray(2);
}

void RenderablePanel::disableAttributes(){
  glDisableVertexAttribArray(2);
  Renderable::disableAttributes();
}

void RenderablePanel::loadToGPU(bool sphericalCoords){
  Renderable::loadToGPU(sphericalCoords);
  glBindVertexArray(vertexArray);
  int texDim = 2;

  glBindBuffer(GL_ARRAY_BUFFER, textureCoordBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    textureCoordinates.size()*sizeof(GLfloat),
    textureCoordinates.data(),
    GL_STATIC_DRAW
  );
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, texDim, GL_FLOAT, GL_FALSE, 0, 0);

  glDisableVertexAttribArray(2);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

GLuint RenderablePanel::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};
