#include <renderablePanel.h>
#include <iostream>
RenderablePanel::RenderablePanel(glm::vec3 pos, float width, float height)
  : pos(pos), width(width), height(height){
  anchor = UI_PLANE_POS;

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

/**
 * Vertex placement
 */
void RenderablePanel::createVertices() {
  // top left
  cartesianVertexData.clear();
  textureCoordinates.clear();
  
  cartesianVertexData.push_back(pos.x);
  cartesianVertexData.push_back(pos.y);
  cartesianVertexData.push_back(pos.z);

  textureCoordinates.push_back(0.0);
  textureCoordinates.push_back(0.0);
  

  // top right
  cartesianVertexData.push_back(pos.x + width);
  cartesianVertexData.push_back(pos.y);
  cartesianVertexData.push_back(pos.z);

  textureCoordinates.push_back(1.0);
  textureCoordinates.push_back(0.0);

  
  // bottom left
  cartesianVertexData.push_back(pos.x);
  cartesianVertexData.push_back(pos.y);
  cartesianVertexData.push_back(pos.z - height);

  textureCoordinates.push_back(0.0);
  textureCoordinates.push_back(1.0);
  
  // bottom right
  cartesianVertexData.push_back(pos.x + width);
  cartesianVertexData.push_back(pos.y);
  cartesianVertexData.push_back(pos.z - height);

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

void RenderablePanel::setPos(glm::vec3 pos) {
  this->pos = pos;
};


void RenderablePanel::setWidth(float width) {
  this->width = width;
};

void RenderablePanel::setHeight(float height) {
  this->height = height;
};

void RenderablePanel::setAnchor (glm::vec3 anchor) {
  assert(anchor.x >= 0.0 && anchor.x <= 1.0 &&
         anchor.z >= 0.0 && anchor.z <= 1.0 &&
         "anchor must be in range [(0.0, 0.0), (1.0, 1.0)]");

  this->anchor.x = anchor.x;
  this->anchor.z = anchor.z;

  createVertices();
  update = true;
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
  std::cout << "uploading texture data to gpu " << textureCoordinates.size()  << std::endl;
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, texDim, GL_FLOAT, GL_FALSE, 0, 0);

  glDisableVertexAttribArray(2);
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

glm::vec3 RenderablePanel::getPos() const {
  return pos;
};

GLuint RenderablePanel::getVertsPerElement() const {
  return 3;
};

float RenderablePanel::getWidth() const {
  return width;
};

float RenderablePanel::getHeight() const {
  return height;
};

glm::vec3 RenderablePanel::getAnchor() const {
  return anchor;
};

