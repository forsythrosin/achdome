#include <renderable.h>

Renderable::Renderable() {
  vertexArray = GL_FALSE;
  positionBuffer = GL_FALSE;
  indexBuffer = GL_FALSE;
  colorBuffer = GL_FALSE;
}

Renderable::~Renderable() {
  delete [] sphericalVertexData;
  delete [] cartesianVertexData;
  delete [] vertexColorData;
  delete [] elementData;
}

void Renderable::attach() {
    // generate vertexArray
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray); // TODO: this row cause segfault sometimes (!)

  // generate buffers
  glGenBuffers(1, &positionBuffer);
  glGenBuffers(1, &indexBuffer);
  glGenBuffers(1, &colorBuffer);
}

void Renderable::detach() {
  glDeleteBuffers(1, &positionBuffer);
  glDeleteBuffers(1, &colorBuffer);
  glDeleteBuffers(1, &indexBuffer);
  glDeleteVertexArrays(1, &vertexArray);
}

const GLfloat * const Renderable::getCartesianVertexData() const {
  return cartesianVertexData;
}

const GLfloat * const Renderable::getSphericalVertexData() const {
  return sphericalVertexData;
}

const GLfloat * const Renderable::getVertexColorData() const {
  return vertexColorData;
}

const GLuint * const Renderable::getElementData() const {
  return elementData;
}

const int Renderable::getVertexCount() const {
  return vertexCount;
}

const int Renderable::getElementCount() const {
  return elementCount;
}

void Renderable::loadToGPU(bool sphericalCoords) {
  // Determine entity dimensions
  int vertexDim = sphericalCoords ? 2 : 3;
  int colorDim = 4;

  const GLfloat* vertexData = vertexDim == 2 ? sphericalVertexData : cartesianVertexData;

  // Upload vertex positions
  glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertexCount*sizeof(GLfloat)*vertexDim,
    vertexData,
    GL_STATIC_DRAW
  );
  // Use buffer as position buffer for shader location = 0
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, vertexDim, GL_FLOAT, GL_FALSE, 0, 0);

  // Upload colors
  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertexCount*sizeof(GLfloat)*colorDim,
    vertexColorData,
    GL_STATIC_DRAW
  );
  // Use buffer as color buffer for shader location = 1
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, colorDim, GL_FLOAT, GL_FALSE, 0, 0);

  // Upload elements (e.g. triangles or line segments)
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    elementCount*sizeof(GLuint)*getVertsPerElement(),
    elementData,
    GL_STATIC_DRAW
  );

  // Unbind
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

