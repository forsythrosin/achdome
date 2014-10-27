#include <renderable.h>

Renderable::~Renderable() {
  delete [] sphericalVertexData;
  delete [] cartesianVertexData;
  delete [] vertexColorData;
  delete [] elementData;
};

const GLfloat * const Renderable::getCartesianVertexData() const {
  return cartesianVertexData;
};

const GLfloat * const Renderable::getSphericalVertexData() const {
  return sphericalVertexData;
};

const GLfloat * const Renderable::getVertexColorData() const {
  return vertexColorData;
};

const GLuint * const Renderable::getElementData() const {
  return elementData;
};

const int Renderable::getVertexCount() const {
  return vertexCount;
};

const int Renderable::getElementCount() const {
  return elementCount;
};

