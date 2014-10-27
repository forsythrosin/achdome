#include <renderablePanel.h>

RenderablePanel::RenderablePanel(glm::vec3 pos, float width, float height)
    : pos(pos), width(width), height(height){
  anchor = UI_PLANE_POS;

  vertexCount = VERTEX_COUNT;
  elementCount = ELEMENT_COUNT;

  sphericalVertexData = new GLfloat[VERTEX_COUNT*2];
  cartesianVertexData = new GLfloat[VERTEX_COUNT*3];
  vertexColorData = new GLfloat[VERTEX_COUNT*4];
  elementData = new GLuint[ELEMENT_COUNT*VERTS_PER_ELEMENT];

  createVertices();
  createElements();
};

RenderablePanel::~RenderablePanel() {};

/**
 * Vertex placement
 */
void RenderablePanel::createVertices() {
  // top left
  cartesianVertexData[0] = pos.x;
  cartesianVertexData[1] = pos.y;
  cartesianVertexData[2] = pos.z;

  // top right
  cartesianVertexData[3] = pos.x + width;
  cartesianVertexData[4] = pos.y;
  cartesianVertexData[5] = pos.z;

  // bottom left
  cartesianVertexData[6] = pos.x;
  cartesianVertexData[7] = pos.y;
  cartesianVertexData[8] = pos.z - height;

  // bottom right
  cartesianVertexData[9] = pos.x + width;
  cartesianVertexData[10] = pos.y;
  cartesianVertexData[11] = pos.z - height;
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
  elementData[0] = topLeft;
  elementData[1] = bottomLeft;
  elementData[2] = topRight;

  // bottom right face
  elementData[3] = topRight;
  elementData[4] = bottomLeft;
  elementData[5] = bottomRight;
};

void RenderablePanel::setColor (glm::vec4 color) {
  int colorDim = 4;
  for (int j = 0; j < VERTEX_COUNT; ++j) {
    for (int i = 0; i < colorDim; ++i) {
      vertexColorData[j*colorDim + i] = color[i];
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

const glm::vec3 RenderablePanel::getPos() const {
  return pos;
};

const float RenderablePanel::getWidth() const {
  return width;
};

const float RenderablePanel::getHeight() const {
  return height;
};

const glm::vec3 RenderablePanel::getAnchor() const {
  return anchor;
};

const GLuint RenderablePanel::getVertsPerElement() const {
  return VERTS_PER_ELEMENT;
};
